#include <regex>

#include "commands.h"

namespace commands = examplebot::commands;

dpp::task<void> commands::emoji_add(const dpp::slashcommand_t &event, std::span<const dpp::command_data_option> options) {
	dpp::cluster *cluster = event.from->creator;

	// Retrieve parameter values
	dpp::snowflake file_id = std::get<dpp::snowflake>(event.get_parameter("file"));
	std::string emoji_name = std::get<std::string>(event.get_parameter("name"));

	// Get the attachment from the resolved list
	const dpp::attachment &attachment = event.command.get_resolved_attachment(file_id);

	// For simplicity for this example we only support PNG
	if (attachment.content_type != "image/png") {
		// While event.co_reply is available, we can just use event.reply, as we will exit the command anyway and don't need to wait on the result
		event.reply("Error: type " + attachment.content_type + " not supported");
		co_return;
	}
	// Send a "<bot> is thinking..." message, to wait on later so we can edit
	dpp::awaitable thinking = event.co_thinking(false); 

	// Download and co_await the result
	dpp::http_request_completion_t response = co_await cluster->co_request(attachment.url, dpp::m_get);

	if (response.status != 200) { // Page didn't send the image
		co_await thinking; // Wait for the thinking response to arrive so we can edit
		event.edit_response("Error: could not download the attachment");
	}
	else {
		// Load the image data in a dpp::emoji
		dpp::emoji emoji(emoji_name);
		emoji.load_image(response.body, dpp::image_type::i_png);

		// Create the emoji and co_await the response
		dpp::confirmation_callback_t confirmation = co_await cluster->co_guild_emoji_create(event.command.guild_id, emoji);

		co_await thinking; // Wait for the thinking response to arrive so we can edit
		if (confirmation.is_error())
			event.edit_response("Error: could not add emoji: " + confirmation.get_error().message);
		else // Success
			event.edit_response("Successfully added " + confirmation.get<dpp::emoji>().get_mention()); // Show the new emoji
	}
}

dpp::task<void> commands::emoji_url(const dpp::slashcommand_t &event, std::span<const dpp::command_data_option> options) {
	using namespace std::string_view_literals;

	if (options.empty() || !std::holds_alternative<std::string>(options[0].value)) {
		event.reply("Error: wrong parameter type");
		co_return;
	}

	const std::string& input = std::get<std::string>(options[0].value);
	std::regex pattern{"^(?:\\s*)<(a?):([a-zA-Z0-9_]+):([0-9]+)>(?:\\s*)$"};
	std::string match;
	std::match_results<std::string::const_iterator> results{};
	if (!std::regex_match(input, results, pattern) || results.size() < 4) {
		event.reply("Please give a custom emoji as the parameter.");
		co_return;
	}
	auto id = results[3];
	std::stringstream ss;

	ss << "https://cdn.discordapp.com/emojis/" << std::string_view{id.first, id.second} << (results[1].length() ? ".gif"sv : ".webp"sv);
	event.reply(ss.str());
}
