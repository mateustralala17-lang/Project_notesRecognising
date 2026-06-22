from basic_pitch.inference import predict
import librosa

def get_formatted_notes(file_path):    
    model_output, midi_data, note_events = predict(file_path)

    note_events.sort(key=lambda x: x[0])
    formatted_notes = []
    for note in note_events:
        start = round(note[0], 2)
        end = round(note[1], 2)
        name = librosa.midi_to_note(note[2])
        formatted_notes.append([start, end, name])
    return formatted_notes