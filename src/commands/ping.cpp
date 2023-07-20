#include "commands.h"

namespace commands = examplebot::commands;

dpp::task<void> commands::ping(const dpp::slashcommand_t &command, std::span<const dpp::command_data_option> options) {
	command.reply("pong");
	co_return;
}
