from scipy.fftpack import fft
from scipy.io import wavfile
from scipy.io.wavfile import write
import matplotlib.pyplot as plt

fs, data = wavfile.read('test1.wav') # Read audio file
a = data.T[0:data.size] #Read only the first channel track
b = [(ele/2**8.)*2-1 for ele in a] #convert from 8-bit
#c = fft(b) # calculate fourier transform of data
d = len(a)//2 #only one channel, not 2 so half length
plt.figure(num=1,figsize=(12,6))
#plt.xlim(0,4000)
plt.plot(abs(b[0]),'b')
plt.savefig('audiofft.jpg',format='jpg')
