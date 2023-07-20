#include <filesystem>

#include <dpp/dpp.h>

#include "commands/commands.h"

int main() {
	std::string token;
	std::ifstream token_file{"token.txt"};

	if (!token_file.good()) {
		std::cerr << "could not read token from " << (std::filesystem::current_path() / "token").string();
		return (1);
	}

	std::getline(token_file, token);

	try {
		dpp::cluster bot(token);

		bot.on_log(dpp::utility::cout_logger());
		bot.on_ready([&bot](const dpp::ready_t &event) {
			if (dpp::run_once<struct register_commands>()) {
				dpp::slashcommand emoji_command{"emoji", "Emoji commands", bot.me.id};

				emoji_command.add_option(
					dpp::command_option{dpp::co_sub_command, "add", "Add an emoji"}
						.add_option({dpp::co_attachment, "file", "File to use for emoji", true})
						.add_option({dpp::co_string, "name", "Name of the emoji to add", true})
				);
				emoji_command.add_option(
					dpp::command_option{dpp::co_sub_command, "url", "Get the URL of an emoji"}
						.add_option({dpp::co_string, "emoji", "Emoji to get the URL of", true})
				);
				bot.global_bulk_command_create({
					{"ping", "Ping me", bot.me.id},
					emoji_command
				});
			}
		});

		bot.on_slashcommand.co_attach([&bot](dpp::slashcommand_t event) -> dpp::task<void> {
			auto subcommand = examplebot::commands::command_handler.get(event);

			if (!subcommand.has_value()) // command not found
				co_return;

			const auto &command = subcommand->first;
			dpp::permission p = event.command.get_resolved_permission(event.command.usr.id);

			if (!p.has(command.permission)) {
				event.reply("You do not have permissions for this command");
				co_return;
			}
			co_await command.handler(event, subcommand->second);
		});

		bot.start(dpp::st_wait);
	}
	catch (const dpp::exception &e) {
		std::cerr << "DPP error : " << e.what();
		return (1);
	}
}
