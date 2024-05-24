from matplotlib.widgets import Slider,Button,CheckButtons
import matplotlib.pyplot as plt
import numpy as np
#y(t) = A ∗ sin(ω ∗ t + φ))
#Дані шуму, які будуть змінюватись відповідними слайдерами та чекбоксом 
noise_v = 0
x = np.arange(-10.0,10.0,0.001)
def noise(noise_mean=None, noise_covariance=None):
    return np.random.normal(noise_mean,noise_covariance, y.shape[0])

#Чиста гармоніка
def harmonic(ampkamplitude=1, frequency=1, phase=0):
    
    y = ampkamplitude*np.sin(frequency*x + phase)
    return y
#Початковий графік
fig, ax = plt.subplots()
ax.set_xlabel('x')
ax.set_ylabel('y')
plt.axis([-10,10,-3,3])

y = harmonic()

line, = ax.plot(x, y)
fig.subplots_adjust(bottom=0.60)

#GUI елементи 
ax_for_ampl = fig.add_axes([0.15, 0.50, 0.70, 0.025])
amplitide_slider = Slider(
    ax=ax_for_ampl,
    label="Amplitude",
    valmin = -3,
    valmax = 3,
    valinit=1
)
ax_for_frequency = fig.add_axes([0.15, 0.40, 0.65, 0.025])
frequency_slider = Slider(
    ax=ax_for_frequency,
    label="Frequency",
    valmin = -10,
    valmax = 10,
    valinit=1
)

ax_for_phase = fig.add_axes([0.15, 0.30, 0.65, 0.025])
phase_slider = Slider(
    ax=ax_for_phase,
    label="Phase",
    valmin = -10,
    valmax = 10,
    valinit=0
)
ax_for_rb = fig.add_axes([0.85, 0.25, 0.10, 0.10])

chkbks = CheckButtons(ax=ax_for_rb,labels=('Noise',))

ax_for_noise_mean = fig.add_axes([0.15, 0.20, 0.65, 0.025])
noise_mean_slider = Slider(
    ax=ax_for_noise_mean,
    label="Noise mean",
    valmin = -3,
    valmax = 3,
    valinit=0
)
ax_for_covariance_noise_ = fig.add_axes([0.15, 0.10, 0.65, 0.025])
noise_covariance_slider = Slider(
    ax=ax_for_covariance_noise_,
    label="Noise covariance",
    valmin = 0,
    valmax = 1,
    valinit = 0,
)
ax_for_rst_but = fig.add_axes([0.85,0.1,0.10,0.05])
btn = Button(
    ax=ax_for_rst_but,
    label='reset'
)
#Функції оновлення графіків
def update_harmonic(val):
    if chkbks.get_checked_labels()==['Noise']:
        global y
        y = harmonic(ampkamplitude=amplitide_slider.val, frequency=frequency_slider.val, phase=phase_slider.val)
        line.set_ydata(noise_v+y)        
        fig.canvas.draw_idle()
    else:
        line.set_ydata(harmonic(ampkamplitude=amplitide_slider.val, frequency=frequency_slider.val, phase=phase_slider.val))
        fig.canvas.draw_idle()

def update_harmonic_with_noise(val):
    if chkbks.get_checked_labels()==['Noise']:
        global noise_v
        y = harmonic(ampkamplitude=amplitide_slider.val, frequency=frequency_slider.val, phase=phase_slider.val)
        noise_v = noise(noise_mean=noise_mean_slider.val,noise_covariance=noise_covariance_slider.val)
        line.set_ydata(noise_v+y)
        fig.canvas.draw_idle()

def update_harmonic_chkb(val):
    if chkbks.get_checked_labels()==['Noise']:
        global noise_v
        y = harmonic(ampkamplitude=amplitide_slider.val, frequency=frequency_slider.val, phase=phase_slider.val)
        noise_v = noise(noise_mean=noise_mean_slider.val,noise_covariance=noise_covariance_slider.val)
        line.set_ydata(y+noise_v)
        fig.canvas.draw_idle()
    else:
        line.set_ydata(harmonic(ampkamplitude=amplitide_slider.val, frequency=frequency_slider.val, phase=phase_slider.val))
        fig.canvas.draw_idle()
#Функція для кнопки reset
def reset(val):
    amplitide_slider.reset()
    frequency_slider.reset()
    phase_slider.reset()
    noise_covariance_slider.reset()
    noise_mean_slider.reset()
    chkbks.clear()
#Реєстрація змін внаслідок взаємодії з елементами
chkbks.on_clicked(update_harmonic_chkb)
amplitide_slider.on_changed(update_harmonic)
frequency_slider.on_changed(update_harmonic)
phase_slider.on_changed(update_harmonic)
noise_covariance_slider.on_changed(update_harmonic_with_noise)
noise_mean_slider.on_changed(update_harmonic_with_noise)
btn.on_clicked(reset)
#Показ графіку
plt.show()