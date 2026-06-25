import os
import io
import math
import streamlit as st
import pandas as pd
from scipy.io import wavfile
import basicPitch

#test_file = "test_audio/fur_elise.mp3"

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

    detected_notes = ["A4", "C5", "E5"] 
    
    st.markdown(render_piano(detected_notes), unsafe_allow_html=True)

    player_container = st.empty()
    table_container = st.empty()


with col_right:
    uploaded_file = st.file_uploader("Завантажити mp3-файл", type=["mp3", "wav"])

    input_mode = st.radio(
        "Режим роботи інтерфейсу:",
        ["Акорди", "Тональності", "Ноти"],
        index=0
    )
        
    if st.button("Розшифрувати", type="primary", use_container_width=True):
        if uploaded_file is not None:
            with st.spinner("Розшифровка..."):
                temp = "temp_audio.mp3"
                with open(temp, "wb") as f:
                    f.write(uploaded_file.getbuffer())

                formatted_notes, audio_data = basicPitch.get_formatted_notes(temp)
                st.metric(label="Average BPM)", value=basicPitch.get_tempo(temp))

                if os.path.exists(temp):
                    os.remove(temp)

                wav_buffer = io.BytesIO()
                wavfile.write(wav_buffer, 22050, audio_data.astype('float32'))
                player_container.audio(wav_buffer, format="audio/wav", start_time=0)
                        
                df = pd.DataFrame(formatted_notes, columns=["Початок", "Кінець", "Нота"])
                table_container.dataframe(df, use_container_width=True)

    st.divider()

    detected_key = "Ля мінор (A minor)"
    st.metric(label="Тональність пісні", value=detected_key)
    st.write("")

    modes = [
        "Мажор (Іонійський)", "Мінор (Еолійський)", "Дорійський", 
        "Фрігійський", "Лідійський", "Міксолідійський", "Локрійський"
    ]
    st.selectbox("Лад", modes, label_visibility="collapsed")

    if input_mode == "Акорди":
        
        
        chords = [
            "C", "G", "D", "A", "E", "B",
            "F#", "Db", "Ab", "Eb", "Bb", "F",
            "Am", "Em", "Bm", "F#m", "C#m", "G#m",
            "Ebm", "Bbm", "Fm", "Cm", "Gm", "Dm"
        ]
        
        for row in range(4):
            cols = st.columns(6)
            for col_index in range(6):
                i = row * 6 + col_index 
                chord = chords[i]
                
                #bg_color = "#ff4b4b" if chord == "Em" else "#2b2b2b"
                bg_color = "#2b2b2b"
                text_color ="#888"
                
                square_html = f"""
                <div style='background-color: {bg_color}; color: {text_color}; 
                            padding: 10px 0; text-align: center; border-radius: 5px; 
                            margin-bottom: 10px; font-weight: bold; border: 1px solid #444;'>
                    {chord}
                </div>
                """
                cols[col_index].markdown(square_html, unsafe_allow_html=True)
    
    if input_mode == "Тональності":
        majors = ["C", "G", "D", "A", "E", "B", "F#", "Db", "Ab", "Eb", "Bb", "F"]
        minors = ["Am", "Em", "Bm", "F#m", "C#m", "G#m", "D#m", "Bbm", "Fm", "Cm", "Gm", "Dm"]

        # 2. Базовый контейнер круга
        html_circle = """
        <div style="position: relative; width: 400px; height: 400px; margin: 0 auto; background-color: #f8f9fa; border-radius: 50%; border: 2px solid #e0e0e0; font-family: sans-serif;">
            <div style="position: absolute; width: 12px; height: 12px; background: #e0e0e0; border-radius: 50%; left: 194px; top: 194px;"></div>
        """

        center_x = 200
        center_y = 200
        radius_major = 150 # Радиус внешнего круга
        radius_minor = 95  # Радиус внутреннего круга

        # 3. Питон сам считает геометрию
        for i in range(12):
            # Угол в градусах (начинаем с -90, чтобы 0 индекс был на 12 часов)
            angle_deg = i * 30 - 90
            # Переводим в радианы для математических функций Питона
            angle_rad = math.radians(angle_deg)
            
            # Координаты для мажорных тональностей (внешний круг)
            x_maj = center_x + math.cos(angle_rad) * radius_major
            y_maj = center_y + math.sin(angle_rad) * radius_major
            
            # Координаты для минорных тональностей (внутренний круг)
            x_min = center_x + math.cos(angle_rad) * radius_minor
            y_min = center_y + math.sin(angle_rad) * radius_minor
            
            # Вычитаем по 20px (половину ширины/высоты кружочка), чтобы они отцентровались ровно по координатам
            # Добавляем мажор (Красный контур)
            html_circle += f"""
            <div style="position: absolute; left: {x_maj - 20}px; top: {y_maj - 20}px; width: 40px; height: 40px; text-align: center; line-height: 40px; font-weight: bold; color: #ff4b4b; background: white; border: 2px solid #ff4b4b; border-radius: 50%; box-shadow: 0 2px 4px rgba(0,0,0,0.1);">{majors[i]}</div>
            """
            
            # Добавляем минор (Синий контур)
            html_circle += f"""
            <div style="position: absolute; left: {x_min - 20}px; top: {y_min - 20}px; width: 40px; height: 40px; text-align: center; line-height: 40px; font-size: 14px; font-weight: bold; color: #4b8bff; background: white; border: 2px solid #4b8bff; border-radius: 50%; box-shadow: 0 2px 4px rgba(0,0,0,0.1);">{minors[i]}</div>
            """

        html_circle += "</div>"

        # 4. Выводим результат в Streamlit
        st.components.v1.html(html_circle, height=450)
    st.divider()
    
    