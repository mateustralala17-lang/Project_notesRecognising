import streamlit as st
import pandas as pd
from basicPitch import get_formatted_notes

test_file = "test_audio/fur_elise.mp3"

st.set_page_config(layout="wide", page_title="Music Transcriber MVP")

col_left, col_right = st.columns([2, 1])

with col_left:
    st.markdown("### Візуалізація нот на фортепіано")
    
    def render_piano(active_notes):
        octave_pattern = [
            ("C", "W"), ("C#", "B"), ("D", "W"), ("D#", "B"), ("E", "W"),
            ("F", "W"), ("F#", "B"), ("G", "W"), ("G#", "B"), ("A", "W"),
            ("A#", "B"),("B", "W")
        ]

        html_keys = ""
        
        for octave in [3, 4, 5]:
            for note_name, key_type in octave_pattern:
                full_note = f"{note_name}{octave}" 
                is_active = full_note in active_notes

                if key_type == "W":
                    bg_color = "#ff4b4b" if is_active else "#ffffff"
                    html_keys += f'<div style="flex-shrink: 0; width: 40px; height: 220px; background-color: {bg_color}; border: 1px solid #333; border-radius: 0 0 5px 5px; position: relative; z-index: 1;"></div>'
                else:
                    bg_color = "#ff4b4b" if is_active else "#111111"
                    html_keys += f'<div style="flex-shrink: 0; width: 24px; height: 140px; background-color: {bg_color}; border: 1px solid #000; border-radius: 0 0 4px 4px; position: relative; z-index: 2; margin-left: -12px; margin-right: -12px;"></div>'

        return f'<div style="display: flex; justify-content: center; align-items: flex-start; background-color: #1e1e1e; padding: 20px 10px; border-radius: 10px; border: 1px solid #444; overflow-x: auto; box-shadow: inset 0 0 10px #000;">{html_keys}</div>'

    detected_notes = ["E3", "B3", "E4"] 
    
    st.markdown(render_piano(detected_notes), unsafe_allow_html=True)


with col_right:
    st.file_uploader("Завантажити mp3-файл", type=["mp3", "wav"])
        
    if st.button("Розшифрувати", type="primary", use_container_width=True):
        with st.spinner("Розшифровка..."):
            result_array = get_formatted_notes(test_file)
                    
            df = pd.DataFrame(result_array, columns=["Початок", "Кінець", "Нота"])
            st.dataframe(df)
    
    st.divider()
    
    detected_key = "E Minor (Мі-мінор)"
    st.metric(label="Тональність пісні", value=detected_key)
    st.write("")

    modes = [
        "Мажор (Іонійський)", "Мінор (Еолійський)", "Дорійський", 
        "Фрігійський", "Лідійський", "Міксолідійський", "Локрійський"
    ]
    st.selectbox("Лад", modes, label_visibility="collapsed")
    
    tonalities = [
        "C", "G", "D", "A", "E", "B",
        "F#", "Db", "Ab", "Eb", "Bb", "F",
        "Am", "Em", "Bm", "F#m", "C#m", "G#m",
        "Ebm", "Bbm", "Fm", "Cm", "Gm", "Dm"
    ]
    
    for row in range(4):
        cols = st.columns(6)
        for col_index in range(6):
            i = row * 6 + col_index 
            tonality = tonalities[i]
            
            bg_color = "#ff4b4b" if tonality == "Em" else "#2b2b2b"
            text_color = "white" if tonality == "Em" else "#888"
            
            square_html = f"""
            <div style='background-color: {bg_color}; color: {text_color}; 
                        padding: 10px 0; text-align: center; border-radius: 5px; 
                        margin-bottom: 10px; font-weight: bold; border: 1px solid #444;'>
                {tonality}
            </div>
            """
            cols[col_index].markdown(square_html, unsafe_allow_html=True)
            
    st.divider()
    
    st.radio(
        "Режим роботи інтерфейсу:",
        ["Тональності", "Акорди", "Ноти"]
    )


