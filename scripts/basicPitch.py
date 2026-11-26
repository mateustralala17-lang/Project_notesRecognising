import sys
import os
os.environ["TF_CPP_MIN_LOG_LEVEL"] = "3"
from basic_pitch.inference import predict
import librosa


def get_formatted_notes(file_path):
    model_output, midi_data, note_events = predict(
        file_path,
        onset_threshold=0.6,
        frame_threshold=0.4,
        minimum_note_length=80.0
    )
    # midi_data.write("debug_output.mid")

    note_events.sort(key=lambda x: x[0])
    formatted_notes = []
    for note in note_events:
        start = round(float(note[0]), 3)
        end = round(float(note[1]), 3)
        pitch = int(note[2])
        formatted_notes.append([start, end, pitch])

    return formatted_notes


def get_tempo(file_path):
    y, sr = librosa.load(file_path)
    tempo, beat_frames = librosa.beat.beat_track(y=y, sr=sr)
    return tempo


if __name__ == "__main__":
    if len(sys.argv) < 2:
        sys.exit(1)

    file_path = sys.argv[1]
    formatted_notes = get_formatted_notes(file_path)
    for start, end, pitch in formatted_notes:
        print(f"NOTE {start:.3f} {end:.3f} {pitch}")