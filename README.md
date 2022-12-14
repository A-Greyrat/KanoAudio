# Kano Audio

a simple library for decoding and playing audio files.  

```
supporting the following formats:

* MP3
* OGG
* WAV
* PCM (16 bit, 44.1kHz, stereo)
* FLAC
```
## Exmaple
![img.png](img.png)

## Usage

```c++
#include "KanoAudio/Audio.h"
#include "KanoAudio/MP3Decoder.h"
#include "KanoAudio/OGGDecoder.h"
#include "KanoAudio/WAVDecoder.h"

using namespace KanoAudio;

int main()
{
    auto audio = Audio::Create();
    auto future = audio->LoadAsync<MP3Decoder>("test.mp3");
    // audio->Load<OGGDecoder>("test.ogg");
    
    // do something else
    std::this_thread::sleep_for(std::chrono::seconds(1));
    
    future.wait();
        
    audio->Play();
    while (audio->IsPlaying())
    {
        Sleep(100);
        // ...
    }
        
    /*
    audio->Pause();
    audio->Stop();
    audio->SetVolume(0.5);
    audio->GetVolume();
    audio->SetLooping(true);
    ...
    */
}

```

## Dependencies

```
Please install the following libraries in vcpkg or your own way:
* libogg (vcpkg install libogg)
* libvorbisfile (vcpkg install libvorbis)
* libmpg123 (vcpkg install mpg123)
* OpenAL (vcpkg install openal-soft)
* libsndfile (vcpkg install libsndfile)

if you want to use the examples, please install the following libraries:
* glfw3 (vcpkg install glfw3)
* glad (vcpkg install glad)
* imgui (vcpkg install imgui)
```

## TODO

```
* add more formats
* add more examples
* add more tests
* async loading (√)
```