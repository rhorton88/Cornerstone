import sounddevice as sd
import pyaudio
import wave
import matplotlib.pyplot as plt
from scipy.fftpack import fft
from scipy.io import wavfile
from scipy.io.wavfile import write

chunk = 4096
sample_format = pyaudio.paInt16
channels = 2
fsr = 44100
seconds = 3
#sd.default.device = 'samson go mic'
#sd.default.device = 2
#myRecording = sd.rec(int(seconds*fsr), samplerate=fsr, channels=2)
#sd.wait()
#write('testAudio.wav', fsr, myRecording)

p = pyaudio.PyAudio()
stream = p.open(format=sample_format,
                channels=channels,
                rate=fsr,
                frames_per_buffer=chunk,
                input=True)
frames = []
for i in range(0,int(fsr/chunk*seconds)):
    data = stream.read(chunk)
    frames.append(data)

stream.stop_stream()
stream.close()
p.terminate()

wf = wave.open('testaudio.wav', 'wb')
wf.setnchannels(channels)
wf.setsamplewidth(p.get_sample_size(sample_format))
wf.setframerate(fsr)
wf.writeframes(b''.join(frames))
wf.close


fs, data = wavfile.read('testAudio.wav') # Read audio file
a = data.T[0:data.size] #Read only the first channel track
b = [(ele/2**8.)*2-1 for ele in a] #convert from 8-bit
c = fft(b) # calculate fourier transform of data
d = len(c)//2 #only one channel, not 2 so half length
plt.figure(num=1,figsize=(12,6))
plt.xlim(0,4000)
plt.plot(abs(c[0]),'r') # plot it in red
plt.savefig('audiofft.png',format='png')
