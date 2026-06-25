
from basic_pitch.inference import predict

import librosa

def get_formatted_notes(file_path):    
    model_output, midi_data, note_events = predict(file_path)
    # midi_data.write("debug_output.mid")
    
    note_events.sort(key=lambda x: x[0])
    formatted_notes = []
    for note in note_events:
        start = round(note[0], 2)
        end = round(note[1], 2)
        name = librosa.midi_to_note(note[2])
        formatted_notes.append([start, end, name])

    audio_data = midi_data.synthesize(fs=22050)
    

    return formatted_notes, audio_data

def get_tempo(file_path):
    y, sr = librosa.load(file_path)
    tempo, beat_frames = librosa.beat.beat_track(y=y, sr=sr)
    return tempo
    