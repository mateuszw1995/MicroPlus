#pragma once
#include "DepthBase.h"
#include "Defines.h"

class CSounds
{
    public:    
    Mix_Music *music;
    Mix_Chunk *chunk;
    CEnum SoundType;
    
    
    void Play(GLint loops, GLint channel = -1){        
        if(SoundType == MUSIC){
            Mix_PlayMusic(music, loops);
        }
        else if(SoundType == CHUNK){
            Mix_PlayChannel(channel, chunk, loops);
        }
	}
    void Stop(GLint channel = -1){
		if(SoundType == MUSIC){
			Mix_HaltMusic();
		}
        else if(SoundType == CHUNK){
			Mix_HaltChannel(channel);
		}
    }
    void Pause(GLint channel = -1){
        if(SoundType == MUSIC){
           Mix_PauseMusic();
        }
        else if(SoundType == CHUNK){
            Mix_Pause(channel);
        }
	}
    void Resume(GLint channel = -1){
        if(SoundType == MUSIC){
            Mix_ResumeMusic();            
        }
        else if(SoundType == CHUNK){
            Mix_Resume(channel);
        }
    }
    
    CSounds(_string filename, CEnum _SoundType) : SoundType(_SoundType){
		if(!Mixer_mode) Mix_OpenAudio( 48000, MIX_DEFAULT_FORMAT, 2, 4096 );
		
		if(_SoundType == MUSIC){
            music = Mix_LoadMUS(filename.c_str());
        }
        else if(_SoundType == CHUNK){
            chunk = Mix_LoadWAV(filename.c_str());
        }
    }
    
    ~CSounds(){
        if(SoundType == MUSIC){
            Mix_FreeMusic(music);
        }
		else if(SoundType == CHUNK){
            Mix_FreeChunk(chunk);
        }
    }
};

