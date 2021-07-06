#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "gmo_sound.h"



char *g_sound_names[NUM_SOUNDS] = {
    "sounds/ocean_noise_surf.wav",
    "sounds/lake_at_night.wav"
};


int32 get_file_size(FILE *fp)
{
     int32 file_size = 0;
     fseek(fp, 0, SEEK_END);
     file_size = ftell(fp);
     rewind(fp);
     return file_size;
}

void list_al_audio_devices(const ALCchar *devices)
{
     const ALCchar *device = devices, *next = device + 1;
     size_t len = 0;
     printf("Devices list:\n");
     printf("-----:\n");
     while (device && *device != '\0' && next && *next != '\0') {
	  printf("%s\n", device);
	  len = strlen(device);
	  device += (len + 1);
	  next += (len + 2);
     }
     printf("-----\n");
}

ALenum get_al_format(Wav_Header wav_header)
{
     ALenum result = AL_FORMAT_STEREO16;//safe default?
     if (wav_header.numChannels == 1) {
	  if (wav_header.bitsPerSample == 8) {
	       result = AL_FORMAT_MONO8;
	  } else if (wav_header.bitsPerSample == 16) {
	       result = AL_FORMAT_MONO16;
	  }
     } else if (wav_header.numChannels == 2) {
	  if (wav_header.bitsPerSample == 8) {
	       result = AL_FORMAT_STEREO8;
	  } else if (wav_header.bitsPerSample == 16) {
	       result = AL_FORMAT_STEREO16;
	  }
     }
     return result;
}

int check_error(void)
{
     int result = 0;
     ALCenum error;
     error = alGetError();
     if (error != AL_NO_ERROR) {
	  result = 1;
     }
     return result;
}

void load_wav(char *file_name, ALvoid **wav_data, Wav_Header *wav_header_in, ALsizei *wav_size)
{
     Wav_Header *wav_header = NULL;
     FILE *wav_ptr;
     if (fopen_s(&wav_ptr, file_name, "rb")) {
	  printf("err open file\n");
     }
     //need to get file size (easier in Windows?)
     int32 file_size = get_file_size(wav_ptr);
     void *file_buffer = (void*)malloc(file_size);
     size_t elements_read = fread(file_buffer, 1, file_size, wav_ptr);
     wav_header = (Wav_Header*)file_buffer;

     *wav_data = (ALvoid*)malloc(wav_header->subChunk2Size);
     fseek(wav_ptr, sizeof(Wav_Header), SEEK_SET);//this may not put us in the correct spot
     fread(*wav_data, 1, wav_header->subChunk2Size, wav_ptr);
     *wav_header_in = *wav_header;
     *wav_size = wav_header->subChunk2Size;
     fclose(wav_ptr);      
}

ALuint al_buffer_from_wav(char *file_name)
{

     //wrap this 
     ALuint al_buffer;

     alGenBuffers((ALuint)1, &al_buffer);
     
     ALsizei al_wav_size, al_wav_freq;

     //wrap this
     ALvoid *al_wav_data;

     Wav_Header wav_header;
     ALsizei wav_size;

     load_wav(file_name, &al_wav_data, &wav_header, &wav_size);
     ALenum al_wav_format = get_al_format(wav_header);
     ALsizei wav_freq = wav_header.sampleRate;     

     alBufferData(al_buffer, al_wav_format, al_wav_data, wav_header.subChunk2Size, wav_freq);
     free(al_wav_data);
     return al_buffer;
     
}

void al_make_source(ALCcontext *al_context, ALuint *al_source, ALuint al_buffer, Vec3 position, Vec3 velocity, real32 pitch, real32 gain, ALboolean looping)
{
    alcMakeContextCurrent(al_context);
    ALfloat listener_orientation[] = {0.0f, 0.0f, 0.0f, 0.0f, -1.0f, 0.0f};     
    alListener3f(AL_POSITION, 0.0f, 0.0f, 0.0f);//this could be updated according to player pos
    alListener3f(AL_VELOCITY, 0.0f, 0.0f, 0.0f);//this could be updated according to player velocity
    alListenerfv(AL_ORIENTATION, listener_orientation);
    alGenSources((ALuint)1, al_source);
    alGetError();
    alSourcef(*al_source, AL_PITCH, pitch);
    ALCenum error;
    if ((error = alGetError()) != AL_NO_ERROR) {
	printf("error loading pitch\n");
	return;
    }
    alSourcef(*al_source, AL_GAIN, gain);
    if ((error = alGetError()) != AL_NO_ERROR) {
	printf("error loading gain\n");
	return;
    }
    alSource3f(*al_source, AL_POSITION, position.x, position.y, position.z);
    if ((error = alGetError()) != AL_NO_ERROR) {
	printf("error loading position\n");
	return;
    }
    alSource3f(*al_source, AL_VELOCITY, velocity.x, velocity.y, velocity.z);
    if ((error = alGetError()) != AL_NO_ERROR) {
	printf("error loading velocity\n");
	return;
    }
    alSourcei(*al_source, AL_LOOPING, AL_FALSE);//better to use this than manual loop
    if ((error = alGetError()) != AL_NO_ERROR) {
	printf("error loading looping\n");
	return;
    }
    alSourcei(*al_source, AL_BUFFER, al_buffer);
    if ((error = alGetError()) != AL_NO_ERROR) {
	printf("error loading gain\n");
	return;
    }
}

ALuint make_source_from_file(ALCcontext *al_context, char *wav_file, Vec3 position, Vec3 velocity, real32 pitch, real32 gain, ALboolean looping)
{
    ALuint buffer = al_buffer_from_wav(wav_file);
    ALuint source;
    al_make_source(al_context, &source, buffer, position, velocity, pitch, gain, looping);
    return source;
}

ALuint *load_static_sources(ALCcontext *al_context)
{
    ALuint *sounds = (ALuint*)malloc(NUM_SOUNDS * sizeof(ALuint));
    Vec3 pos = vec3_init(0.0f, 0.0f, 0.0f);
    for (uint32 i = 0; i < NUM_SOUNDS; i++) {
	sounds[i] = make_source_from_file(al_context, g_sound_names[i], pos, pos, 1.0f, 1.0f, AL_TRUE);
    }

    return sounds;
}

ALuint *load_sound_buffers(ALCcontext *al_context)
{
    ALuint *buffers = (ALuint*)malloc(NUM_SOUNDS * sizeof(ALuint));

    for (uint32 i = 0; i < NUM_SOUNDS; i++) {
	buffers[i] = al_buffer_from_wav(g_sound_names[i]);
    }

    return buffers;
}

void play_sound_source(ALuint source)
{
    	ALint al_source_state;
	alGetSourcei(source, AL_SOURCE_STATE, &al_source_state);
	//printf("al step source is definitely %d\n", al_step_source);
	if (al_source_state == AL_STOPPED) {
	    //printf("al step source is stopped %d\n", al_source);
	    alSourcePlay(source);
	    al_source_state = AL_PLAYING;
	} else if (al_source_state == AL_PLAYING){
	    //printf("al step source is playing %d\n", al_source);
	} else if (al_source_state == AL_INITIAL){
	    //printf("al step source is initial %d\n", al_source);
	    alSourcePlay(source);
	} else if (al_source_state == AL_PAUSED){
	    //printf("al step source is pausedd %d\n", al_source);
	}
	    
}
