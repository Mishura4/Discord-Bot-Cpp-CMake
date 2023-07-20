#ifndef COMMANDS_H_
#define COMMANDS_H_

#include <unordered_map>
#include <functional>
#include <span>

#include <dpp/dpp.h>

namespace examplebot {
	namespace commands {
		struct command {
			using handler_t = std::function<dpp::task<void>(const dpp::slashcommand_t &, std::span<const dpp::command_data_option>)>;

			handler_t handler;
			dpp::permission permission{dpp::p_administrator};
			std::unordered_map<std::string, command> subcommands{};

			command(std::initializer_list<std::pair<const std::string, command>> subcommands_) :
				handler(nullptr),
				subcommands(subcommands_)
			{
			}

			command(handler_t handler_, dpp::permission permission_) :
				handler(std::move(handler_)),
				permission(permission_)
			{
			}
		};

		struct command_handler_t {
			using resolved_subcommand = std::optional<std::pair<const command &, std::span<const dpp::command_data_option>>>;

			std::unordered_map<std::string, command> commands;

			command_handler_t(std::initializer_list<std::pair<const std::string, command>> init) :
				commands(init)
			{
			}

			resolved_subcommand get(const dpp::slashcommand_t &event) const;
		};

		dpp::task<void> ping(const dpp::slashcommand_t &command, std::span<const dpp::command_data_option> options);

		dpp::task<void> emoji_add(const dpp::slashcommand_t &command, std::span<const dpp::command_data_option> options);
		dpp::task<void> emoji_url(const dpp::slashcommand_t &command, std::span<const dpp::command_data_option> options);

		inline const auto command_handler = command_handler_t{
			{"ping", {&ping, dpp::p_send_messages}},
			{"emoji", {
				{"add", {&emoji_add, dpp::p_manage_emojis_and_stickers}},
				{"url", {&emoji_url, dpp::p_send_messages | dpp::p_embed_links}}
			}}
		};
	}
}

#endif /* COMMANDS_H_ */
