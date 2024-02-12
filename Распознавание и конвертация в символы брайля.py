# Скрипт "Голосовое управление принтером"
# Кодировка ru-RU.UTF-8, Платформа OrangePI Zero 2
# Импортируем библиотеки
import speech_recognition # Распознавания речи
import sounddevice # Чтобы аудиодрайвер не сыпал сообщения в консоль
import re # Поддержка регулярных выражений
from playsound import playsound # Аудиопроигрывание
import pexpect # Для запуска дочерних процессов
import os

# Настройки
sr = speech_recognition.Recognizer() # Создаем объект распознавания речи
sr.pause_threshold=1 # Время паузы после которой будет передача на распознавание
durations=0.5 # Прослушивание окружающего шума в секундах
ID_mic=5 # ID номер устройства микрофона. Для OrangePI Zero 2 = 5
rec=0 # Признак записи сообщения. 0 - запись не идёт, 1 - идёт
result = "" # Переменная хранящая результат распознавания голоса
child = pexpect.spawn('pacmd set-default-sink \'1\'') # Переключаем вывод звука на девайс №1 (hw:2,0) OrangePI Zero 2
x = ''
y = '' # х и у переменные для сохранения
braille = ['101100 ','010000 ','010100 ','110000 ','111000 ','010010 ','110100 ','111100 ','011100 ','100100 ', #значения букв в системе брайля
		#       а        б        в         г          д         е        ё         ж         з          и        й
			'010000 ','010100 ','101110 ','111100 ','111000 ','011000 ','010010 ','101100 ','011011 ','100100 ', '110111 ',
			#   к        л          м       н        о           п         р        с          т         у         ф       
			'010001 ','010101 ','110001 ','111001 ','011001 ','110101 ','011101 ','100101 ','101101 ','010011 ', '110100 ',
			#    х        ц         ч         ш         щ        ъ            ы         ь        э           ю          я
			'011100 ','110000 ','111101 ','011010 ','110011 ','011111 ', '100111 ', '101111 ', '100110 ', '011110 ', '110110 ',
			'001100', 'm', '001110', '000101', '010001', '001100']
R = ['0','1','2','3','4','5','6','7','8','9', #символы конвертации речи в текст
			'а','б','в','г','д','е','ё','ж','з','и', 'й',
			'к','л','м', 'н', 'о','п','р','с','т','у','ф',
			'х','ц','ч','ш','щ','ъ', 'ы', 'ь', 'э', 'ю', 'я',
			':',' ', '+', '-', '*', '/']

def Recognize_command(): # Функция распознавания голоса на лету через сервис Google
    with speech_recognition.Microphone(ID_mic) as mic: # Использование микрофона с адресом id=5
       #print('Говорите') # Для отладки
       sr.adjust_for_ambient_noise(source=mic, duration=durations) # Микрофон и значение durations
       playsound('audio/start.wav')
       audio = sr.listen(source=mic) # Записываем речь с микрофона
       playsound('audio/stop.wav')
       try:
          query = sr.recognize_google(audio_data=audio, language='ru-RU').lower() # API google распознает голос в текст
          if '.' in query:
             query = query.replace(".", " точка ")
       except speech_recognition.UnknownValueError: # В случае появления ошибки "UnknownValueError" не останавливать программу
          query = ('Error1') # Записываем текст ошибки в результат
       return query # Возвращаем результат с распознанной речью или ошибкой

def Braille2RU(BrailleText) :
    x = ''.join([R[braille.index(fi)] for ch in BrailleText for fi in braille if ch == fi])
    return x
   
def RUSSIAN2Braiile(russianText) :
    y = ''.join([braille[R.index(fi)] for ch in russianText for fi in R if ch == fi])
    return y
 
print ('Голосовой модуль управления принтером Брайля. Скажи "помощь" чтобы узнать команды')
print ('Готов к работе')

while true: # Бесконечный цикл
   query = Recognize_command() # Запускаем функцию распознавания голоса с возвратом значения в переменную

   if rec==0:
      match query: # Сопоставление распознанного текста со значениями и запуск действий
         case 'выключить' | 'выход':
            print ('Выключаюсь (Выход из программы)')
            playsound('audio/off.wav')
            exit() # Остановка программы
         case 'записать' | 'запись' | 'записываем':
            print ('Записываем сообщение')
            rec=1
            query = ""
            result = ""
            playsound('audio/zapis.wav')
         case 'стоп':
            print ('Пока нечего останавливать')
            playsound('audio/nostop.wav')
         case 'прослушать' | 'послушать' | 'воспроизвести' | 'play' | 'проиграть':
            if result != '':
               print ('Воспроизвожу распознанную запись:', result)
               playsound('audio/play.wav')
               os.system("echo \"" + result + "\" | festival --tts")
            else:
               print ('Пока нечего слушать')
               playsound('audio/nechegosluchat.wav')
         case 'печать' | 'напечатать':
            print ('Печатаю:')
            print ('=================================================================================================')
            e = RUSSIAN2Braiile(result)
            r = open(('/home/orangepi/bot/cal.txt','w', encoding='utf-8').write(e)
            os.system("sudo gcc C_main.cpp -o blink -lWiringPi.h -lStepper.h -liostream -lstring -lfstream")
            print (result)
            print ('=================================================================================================')
            playsound('audio/print.wav')
         case 'Error1':
            print ('Не разобрал')
            playsound('audio/neponyal.wav')
         case 'помощь' | 'help':
            print ('Справка по голосовому управлению принтером Брайля. Команды:')
            print ('\"Запись\" - начинает запись сообщения для дальнейшей печати на принтере')
            print ('\"Стоп\" - останавливает запись. Во время записи другие команды не работают')
            print ('\"Прослушать\" - Читает распознанное сообщение')
            print ('\"Печать\" - Печатает распознанное сообщение')
            print ('\"Выключить\" - Выход из программы')
            playsound('audio/help.wav')
         case _: # Если ни одно действие не совпало
            print('У меня нет действий на команду:', query)
            playsound('audio/nocommand.wav')
   if rec==1:
      if "Error1" in query:
         query = re.compile('(\s*)Error1(\s*)').sub('\\1''\\2', query)
      if "стоп" in query:
         query = re.compile('(\s*)стоп(\s*)').sub('\\1''\\2', query)
         result = result + ' ' + query
         print ('Запись остановлена. Распознанный текст:', result)
         playsound('audio/zapisal.wav')
         rec=0
      elif "stop" in query:
         query = re.compile('(\s*)stop(\s*)').sub('\\1''\\2', query)
         result = result + ' ' + query
         print ('Запись остановлена. Распознанный текст:', result)
         playsound('audio/zapisal.wav')
         rec=0
      else:
         if query != '':
            result = result + ' ' + query
            print (result) 



