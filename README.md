# text2Speech

convert a text string to audfio ( mp3 file ) using elevenLabs API, needs environment variable with API key set ELEVENLABS_API_KEY


# Usage: Text to Speech Converter [-h] [--text VAR] [--output VAR] [--model-id VAR] [--stability VAR] [--similarity-boost VAR] [--style VAR] [--use-speaker-boost] [--optimize-streaming-latency] [--output-format VAR] [--list-voices] [--voice-id VAR]

Optional arguments:

  -h, --help                    shows help message and exits
  
  -v, --version                 prints version information and exits
  
  --text                        Text to be converted to speech
  
  -o, --output                  Name of the output MP3 file [default: "output.mp3"]
  
  --model-id                    Model ID for speech synthesis [default: "eleven_monolingual_v1"]
  
  --stability                   Stability setting for voice [default: 0]
  
  --similarity-boost            Similarity boost setting [default: 0]
  
  --style                       Style setting for voice [default: 0]
  
  --use-speaker-boost           Whether to use speaker boost
  
  --optimize-streaming-latency  Optimize streaming latency
  
  --output-format               Output format [default: "mp3_44100_128"]
  
  --list-voices                 Fetch and display available voices
  
  --voice-id                    Voice to use (see --list-voices [default: "21m00Tcm4TlvDq8ikWAM"]
  


# API

  https://api.elevenlabs.io/docs
