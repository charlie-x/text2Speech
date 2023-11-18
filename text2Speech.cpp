///
/// Use the elevenLabs API for text to speech
/// saves out as an mp3 file
/// 
#include <argparse/argparse.hpp>
#include <iostream>
#include <fstream>
#include <cpr/cpr.h>
#include <nlohmann/json.hpp>
#include <cstdlib>
#include <spdlog/spdlog.h>

#pragma comment(lib,"ws2_32.lib")
#pragma comment(lib,"crypt32.lib")

using json = nlohmann::json;

/// <summary>
/// safely retrieve the API key from the environment variable
/// </summary>
/// <returns>env variable or not found string</returns>
std::string getApiKey() {

#ifdef _WIN32
	char* value = nullptr;
	size_t size = 0;
	// _dupenv_s is a more secure version of getenv.
	// It duplicates the environment variable and stores it in 'value'
	const errno_t err = _dupenv_s(&value, &size, "ELEVENLABS_API_KEY");
	if (err || value == nullptr) return "API_KEY_NOT_FOUND";

	return value;

#else

	char *envvar =getenv("ELEVENLABS_API_KEY") ;	
	
	if ( envvar == NULL) {
		return "API_KEY_NOT_FOUND";
	}

	std::string senvar( envvar );

	free(envvar);

	return  senvar;
#endif

}

//
// function to convert text to speech using ElevenLabs API
// see https://api.elevenlabs.io/docs for details on each
//
std::string textToSpeech(
	const std::string& text,
	const std::string& apiKey,
	const std::string& modelId,
	int stability,
	int similarityBoost,
	int style,
	bool useSpeakerBoost,
	const std::string& baseUrl,
	const std::string& queryParams
) {
	std::string url = baseUrl + queryParams;

	// JSON object for the POST data
	json postData = {
		{"text", text},
		{"model_id", modelId},
		{"voice_settings", {
			{"stability", stability},
			{"similarity_boost", similarityBoost},
			{"style", style},
			{"use_speaker_boost", useSpeakerBoost}
		}}
	};

	// headers for the request
	cpr::Header headers = {
		{"xi-api-key" , apiKey},
		{"Content-Type", "application/json"},
		{"accept", "audio/mpeg"}
	};

	spdlog::trace("{} ", url);
	spdlog::trace("post {}", postData.dump() );

	// make a POST request
	cpr::Response r = cpr::Post(
		cpr::Url{ url },
		cpr::Body{ postData.dump() },
		headers
	);

	// check for errors
	if (r.status_code != 200) {
		std::cerr << "Request failed with status code: " << r.status_code << std::endl << r.text << std::endl;
		return "";
	}

	return r.text;
}



/// <summary>
/// list the voices available from v1
/// </summary>
/// <returns>0 on pass, < 0 for errors</returns>
int listAvailableVoices() {
	try {

		
		cpr::Response r = cpr::Get(cpr::Url{ "https://api.elevenlabs.io/v1/voices" });

		if (!r.error.message.empty()) {
			std::cerr << "Get error: " << r.error.message << std::endl;
			return -1; 
		}

		if (r.status_code == 200) {

			json j = json::parse(r.text);

			for (const auto& voice : j["voices"]) {

				std::string voiceId = voice["voice_id"].is_null() ? "N/A" : voice["voice_id"].get<std::string>();
				std::string name = voice["name"].is_null() ? "N/A" : voice["name"].get<std::string>();
				std::string category = voice["category"].is_null() ? "N/A" : voice["category"].get<std::string>();
				std::string description = voice["description"].is_null() ? "N/A" : voice["description"].get<std::string>();
				std::string previewUrl = voice["preview_url"].is_null() ? "N/A" : voice["preview_url"].get<std::string>();

				
				// print voice details
				std::cout << "Voice ID: " << voiceId << "\n";
				std::cout << "Name: " << name << "\n";

				// processing labels
				if (!voice["labels"].is_null()) {
					std::string labelAccent = voice["labels"]["accent"].is_null() ? "N/A" : voice["labels"]["accent"].get<std::string>();
					std::string labelDescription;
					
					// Bug in elevenlabs JSON sometimes its "description" and others its "description "
					if (voice.contains("labels") && voice["labels"].is_object() && voice["labels"].contains("description")) {
						labelDescription = voice["labels"]["description"].is_null() ? "N/A" : voice["labels"]["description"].get<std::string>();
					}
					else {
						labelDescription = "N/A";
					}
					if (voice.contains("labels") && voice["labels"].is_object() && voice["labels"].contains("description ")) {
						labelDescription = voice["labels"]["description "].is_null() ? "N/A" : voice["labels"]["description "].get<std::string>();
					}
					else {
						labelDescription = "N/A";
					}


					std::cout << "Label Accent: " << labelAccent << "\n";
					std::cout << "Label Description: " << labelDescription << "\n";
				}
				std::cout << "Category: " << category << "\n";
				std::cout << "Description: " << description << "\n";
				std::cout << "Preview URL: " << previewUrl << "\n";
				std::cout << "-----------------------\n";
			}
		}
		else {
			std::cerr << "HTTP request failed. Status code: " << r.status_code << std::endl;
			return -2; 
		}

		std::cout << "Use the Voice ID with the command line option --voice-id" << std::endl;
	}

	catch (const json::parse_error& e) {
		std::cerr << "JSON parsing error: " << e.what() << std::endl;
		return -4; 
	}
	catch (const std::exception& e) {
		std::cerr << "An unexpected error occurred: " << e.what() << std::endl;
		return -5; 
	}

	return 0;
}

int main(int argc, char** argv) {
	argparse::ArgumentParser program("Text to Speech Converter");

	program.add_argument("--text")
		.help("Text to be converted to speech");

	program.add_argument("-o", "--output")
		.default_value(std::string("output.mp3"))
		.help("Name of the output MP3 file");
	
	program.add_argument("--model-id")
		.default_value(std::string("eleven_monolingual_v1"))
		.help("Model ID for speech synthesis");

	program.add_argument("--stability")
		.default_value(0)
		.scan<'i', int>()
		.help("Stability setting for voice");

	program.add_argument("--similarity-boost")
		.default_value(0)
		.scan<'i', int>()
		.help("Similarity boost setting");

	program.add_argument("--style")
		.default_value(0)
		.scan<'i', int>()
		.help("Style setting for voice");

	program.add_argument("--use-speaker-boost")
		.default_value(true)
		.implicit_value(false)
		.help("Whether to use speaker boost");

	program.add_argument("--optimize-streaming-latency")
		.default_value(true)
		.implicit_value(false)
		.help("Optimize streaming latency");

	program.add_argument("--output-format")
		.default_value(std::string("mp3_44100_128"))
		.help("Output format");
	
	program.add_argument("--list-voices")
		.default_value(false)
		.implicit_value(true)
		.help("Fetch and display available voices");

	program.add_argument("--voice-id")
		.default_value(std::string("21m00Tcm4TlvDq8ikWAM"))
		.help("Voice to use (see --list-voices");

	
	// parse command-line arguments
	try {
		program.parse_args(argc, argv);
	}
	catch (const std::runtime_error& err) {
		std::cerr << err.what() << std::endl;
		std::cerr << program;
		return 1;
	}

	// fetch API key from environment variable
	std::string apiKey(getApiKey());
	if (apiKey.length() == 0) {
		std::cerr << "Environment variable 'ELEVENLABS_API_KEY' not found." << std::endl;
		return 1;
	}
	
	// just list voices available
	if ( program.get<bool>("--list-voices" )) {
		
		listAvailableVoices();

		exit(0);
	}

	// extract arguments
	try {
		std::string text = program.get<std::string>("--text");
		std::string outputFile = program.get<std::string>("--output");
		std::string modelId = program.get<std::string>("--model-id");


		int stability = program.get<int>("--stability");
		int similarityBoost = program.get<int>("--similarity-boost");
		int style = program.get<int>("--style");
		bool useSpeakerBoost = program.get<bool>("--use-speaker-boost");
		int optimize_streaming_latency = program.get<bool>("--optimize-streaming-latency");

		std::string output_format = program.get<std::string>("--output-format");
		std::string voice_id = program.get<std::string>("--voice-id");


		// base API url + voice_id
		std::string baseUrl = "https://api.elevenlabs.io/v1/text-to-speech/" + voice_id;

		std::string queryParams = "?optimize_streaming_latency=" +
			std::to_string(optimize_streaming_latency) +
			"&output_format=" + output_format;
		std::string speech = textToSpeech(
			text, apiKey, modelId, stability, similarityBoost, style, useSpeakerBoost, baseUrl, queryParams
		);


		// if we got a response, write it to a file
		// @todo : add support for other formats
		if( speech.length()) {
		
			// save to MP3 file
			std::ofstream mp3File(outputFile, std::ios::binary);
			if (mp3File.is_open()) {
				mp3File.write(speech.c_str(), speech.size());
				mp3File.close();
				std::cout << "MP3 file created successfully at " << outputFile << std::endl;
			}
			else {
				std::cerr << "Unable to open file for writing: " << outputFile << std::endl;
				return 1;
			}
		}
	}
	catch (const std::exception& err) {
		std::cerr << err.what() << std::endl;
		std::cerr << program;
		std::exit(1);
	}

	return 0;
}
