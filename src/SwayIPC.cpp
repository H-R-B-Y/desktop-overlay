
#include "SwayIPC.hpp"

namespace
{
	struct __attribute__((packed)) IpcHeader
	{
		char magic[6];
		uint32_t length;
		uint32_t type;
	};

	std::string json_quote(const std::string &in)
	{
		std::string out;
		out.reserve(in.size() + 2);
		out.push_back('"');
		for (char c : in)
		{
			if (c == '\\' || c == '"')
				out.push_back('\\');
			out.push_back(c);
		}
		out.push_back('"');
		return out;
	}
}

bool SwayIpcClient::start(void)
{
	return start({"workspace", "output"});
}

bool SwayIpcClient::start(const std::vector<std::string> &subs)
{
	const char	*sock_path = std::getenv("SWAYSOCK");
	if (!sock_path || !*sock_path)
	{
		std::cerr << "SWAYSOCK is not set" << std::endl;
		return false;
	}
	fd_ = socket(AF_UNIX, SOCK_STREAM, 0);
	if (fd_ < 0)
	{
		perror("socket");
		return false;
	}

	sockaddr_un	addr{};
	addr.sun_family = AF_UNIX;
	std::strncpy(addr.sun_path, sock_path, sizeof(addr.sun_path) - 1);
	if (connect(fd_, reinterpret_cast<sockaddr *>(&addr), sizeof(addr)) < 0)
	{
		perror("connect");
		close(fd_);
		fd_ = -1;
		return false;
	}
	if (!send_subscribe(subs))
	{
		std::cerr << "Failed to subscribe to sway events" << std::endl;
		close(fd_);
		fd_ = -1;
		return false;
	}
	channel_ = g_io_channel_unix_new(fd_);
	g_io_channel_set_encoding(channel_, nullptr, nullptr);
	g_io_channel_set_buffered(channel_, FALSE);

	watch_id_ = g_io_add_watch(
		channel_,
		static_cast<GIOCondition>(G_IO_IN | G_IO_HUP | G_IO_ERR),
		&SwayIpcClient::io_cb,
		this
	);
	return true;
}

SwayIpcClient::~SwayIpcClient()
{
	if (watch_id_ != 0)
	{
		g_source_remove(watch_id_);
	}
	if (channel_)
	{
		g_io_channel_unref(channel_);
	}
	if (fd_ >= 0)
	{
		close(fd_);
	}
}

void	SwayIpcClient::set_on_event(std::function<void(const std::string &)> cb)
{
	on_event_ = std::move(cb);
}

gboolean	SwayIpcClient::io_cb(GIOChannel *source, GIOCondition condition, gpointer user_data)
{
	auto *self = static_cast<SwayIpcClient *>(user_data);
	
	if (condition & (G_IO_HUP | G_IO_ERR))
	{
		std::cerr << "sway IPC disconnected" << std::endl;
		return FALSE;
	}
	GIOStatus status = G_IO_STATUS_NORMAL;
	gchar	buf[4096];
	gsize	bytes_read = 0;
	GError	*error = nullptr;

	status = g_io_channel_read_chars(source, buf, sizeof(buf), &bytes_read, &error);
	if (error)
	{
		std::cerr << "read error: " << error->message << std::endl;
		g_error_free(error);
		return FALSE;
	}
	if (status == G_IO_STATUS_EOF)
	{
		return FALSE;
	}
	if (bytes_read > 0)
	{
		self->pending_.append(buf, bytes_read);
		self->drain_messages();
	}
	return TRUE;
}

void	SwayIpcClient::drain_messages(void)
{
	while (true)
	{
		if (pending_.size() < 14)
		{
			return ;
		}
		if (std::memcmp(pending_.data(), IPC_MAGIC, 6) != 0)
		{
			pending_.clear();
			return ;
		}
		uint32_t	payload_len = 0;
		uint32_t	message_type = 0;
		std::memcpy(&payload_len, pending_.data() + 6, 4);
		std::memcpy(&message_type, pending_.data() + 10, 4);
		if (pending_.size() < 14 + payload_len)
		{
			return ;
		}
		std::string payload = pending_.substr(14, payload_len);
		pending_.erase(0, 14 + payload_len);
		if (on_event_)
		{
			on_event_(payload);
		}
	}
}

bool	SwayIpcClient::send_subscribe(const std::vector<std::string> &subs)
{
	if (fd_ < 0)
		return false;

	std::string payload = "[";
	for (size_t i = 0; i < subs.size(); ++i)
	{
		if (i != 0)
			payload += ",";
		payload += json_quote(subs[i]);
	}
	payload += "]";

	IpcHeader header{};
	std::memcpy(header.magic, IPC_MAGIC, sizeof(header.magic));
	header.length = static_cast<uint32_t>(payload.size());
	header.type = static_cast<uint32_t>(IpcType::Subscribe);

	if (!write_exact(fd_, &header, sizeof(header)))
		return false;
	if (!payload.empty() && !write_exact(fd_, payload.data(), payload.size()))
		return false;

	IpcHeader reply{};
	if (!read_exact(fd_, &reply, sizeof(reply)))
		return false;
	if (std::memcmp(reply.magic, IPC_MAGIC, sizeof(reply.magic)) != 0)
		return false;

	std::string reply_payload(reply.length, '\0');
	if (reply.length > 0 && !read_exact(fd_, reply_payload.data(), reply.length))
		return false;

	return (reply_payload.find("\"success\":true") != std::string::npos
		|| reply_payload.find("\"success\": true") != std::string::npos);
}



// for the command:


bool	SwayCommand::send_command(IpcType type, const std::string &sway_cmd, std::function<void(bool, std::string)> cb)
{
	bool			returncode = false;
	IpcHeader		header{};
	IpcHeader		reply{};
	std::string		reply_payload;
	const uint32_t	expected_reply_type = static_cast<uint32_t>(type);

	if (!_start())
	{
		reply_payload = "failed to connect to SWAYSOCK";
		goto exit_;
	}
	std::memcpy(header.magic, IPC_MAGIC, sizeof(header.magic));
	header.length = static_cast<uint32_t>(sway_cmd.length());
	header.type = static_cast<uint32_t>(type);
	if (!write_exact(fd_, &header, sizeof(header)))
	{
		reply_payload = "failed to write IPC header";
		goto exit_;
	}
	if (!sway_cmd.empty() && !write_exact(fd_, sway_cmd.data(), sway_cmd.size()))
	{
		reply_payload = "failed to write IPC payload";
		goto exit_;
	}
	if (!read_exact(fd_, &reply, sizeof(reply)))
	{
		reply_payload = "failed to read IPC reply header";
		goto exit_;
	}
	if (std::memcmp(reply.magic, IPC_MAGIC, sizeof(reply.magic)) != 0)
	{
		reply_payload = "invalid IPC magic in reply";
		goto exit_;
	}
	if (reply.type != expected_reply_type)
	{
		reply_payload = "unexpected reply type";
		goto exit_;
	}
	reply_payload = std::string(reply.length, '\0');
	if (reply.length > 0 && !read_exact(fd_, reply_payload.data(), reply.length))
	{
		reply_payload = "failed to read IPC reply payload";
		goto exit_;
	}
	returncode = true;
exit_:
	if (cb)
		cb(returncode, reply_payload);
	_stop();
	return (returncode);
}

SwayCommand::~SwayCommand()
{
	_stop();
}

bool SwayCommand::_start(void)
{
	const char	*sock_path = std::getenv("SWAYSOCK");
	if (!sock_path || !*sock_path)
	{
		std::cerr << "SWAYSOCK is not set" << std::endl;
		return false;
	}

	fd_ = socket(AF_UNIX, SOCK_STREAM, 0);
	if (fd_ < 0)
	{
		perror("socket");
		return false;
	}

	sockaddr_un	addr{};
	addr.sun_family = AF_UNIX;
	std::strncpy(addr.sun_path, sock_path, sizeof(addr.sun_path) - 1);
	if (connect(fd_, reinterpret_cast<sockaddr *>(&addr), sizeof(addr)) < 0)
	{
		perror("connect");
		close(fd_);
		fd_ = -1;
		return false;
	}
	return true;
}

bool SwayCommand::_stop(void)
{
	if (fd_ >= 0)
	{
		close(fd_);
		fd_ = -1;
	}
	return true;
}
