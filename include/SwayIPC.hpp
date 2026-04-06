
// double check that these are all valid / used in this file

#include <arpa/inet.h>
#include <errno.h>
#include <fcntl.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <unistd.h>

#include <cstdint>
#include <cstdlib>
#include <cstring>
#include <iostream>
#include <optional>
#include <string>
#include <string_view>
#include <vector>

#include <glib.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <unistd.h>
#include <cstdlib>
#include <cstring>
#include <string>
#include <vector>
#include <iostream>
#include <functional>

static constexpr char IPC_MAGIC[] = "i3-ipc";

enum class IpcType : uint32_t 
{
	Command = 0,
	GetWorkspaces = 1,
	Subscribe = 2,
	GetOutputs = 3,
};

struct IpcMessage
{
	uint32_t type;
	std::string payload;
};

static bool read_exact(int fd, void *buf, size_t len)
{
	auto		*out		= static_cast<char *>(buf);
	size_t		done		= 0;

	while (done < len)
	{
		ssize_t	n = read(fd, out + done, len - done);
		if (n == 0) return false;
		if (n < 0)
		{
			if (errno == EINTR) // what about EAGAIN or others?
				continue ;
			return false;
		}
		done += static_cast<size_t>(n);
	}
	return true;
}

static bool write_exact(int fd, const void *buf, size_t len)
{
	const auto		*in			= static_cast<const char *>(buf);
	size_t			done		= 0;

	while (done < len)
	{
		ssize_t		n			= write(fd, in + done, len - done);
		if (n < 0)
		{
			if (errno == EINTR)
				continue ;
			return false;
		}
		done += static_cast<size_t>(n);
	}
	return true;
}

class SwayIpcClient
{
public:
	bool start(void);
	bool start(const std::vector<std::string> &subs);
	~SwayIpcClient();
	void	set_on_event(std::function<void(const std::string &)> cb);
protected:
private:

	static gboolean	io_cb(GIOChannel *source, GIOCondition condition, gpointer user_data);
	void drain_messages(void);

	bool send_subscribe(const std::vector<std::string> &subs);

	int fd_ = -1;
	GIOChannel *channel_ = nullptr;
	guint watch_id_ = 0;
	std::string pending_;
	std::function<void(const std::string &)> on_event_;
};

class SwayCommand
{
public:
	bool	send_command(IpcType type, const std::string &sway_cmd, std::function<void(bool, std::string)> cb);
	~SwayCommand();
private:
	bool	_start(void);
	bool	_stop(void);
	int	fd_ = -1;
};