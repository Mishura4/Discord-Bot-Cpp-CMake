#include <span>

#include "commands.h"

using namespace examplebot::commands;

namespace {
	command_handler_t::resolved_subcommand resolve_subcommands(const command &c, std::span<const dpp::command_data_option> options) {
		for (const dpp::command_data_option &option : options)
		{
			if (option.type == dpp::co_sub_command) {
				auto subcommand = c.subcommands.find(option.name);

				if (subcommand == c.subcommands.end())
					return {std::nullopt};
				return (resolve_subcommands(subcommand->second, option.options));
			}
		}
		return {{c, options}};
	}
}

auto command_handler_t::get(const dpp::slashcommand_t &event) const -> resolved_subcommand {
	const auto &interaction = std::get<dpp::command_interaction>(event.command.data);

	auto elem = commands.find(interaction.name);

	if (elem == commands.end())
		return {std::nullopt};

	return resolve_subcommands(elem->second, interaction.options);
}
