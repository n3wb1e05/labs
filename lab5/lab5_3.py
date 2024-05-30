import numpy as np
import scipy.signal as spi
from bokeh.layouts import column
from bokeh.models import Slider, Button,Range1d,CheckboxButtonGroup,ColumnDataSource
from bokeh.plotting import figure,curdoc

#y(t) = A ∗ sin(ω ∗ t + φ))
def noise(noise_mean=0, noise_covariance=0):
    return np.random.normal(noise_mean, noise_covariance, x.shape[0])

def harmonic(x,amplitude=1, frequency=1, phase=0):
    return amplitude * np.sin(frequency * x + phase)

def noise_filter(arr, k=1):
    filtered_y = np.convolve(arr, np.ones(k) / k, mode='same')
    for i in range(len(filtered_y)):
        neighborhood = arr[max(0, i - k // 2):min(len(arr), i + k // 2 + 1)]
        filtered_y[i] = np.median(neighborhood)
    return filtered_y

x = np.arange(-10.0, 10.0, 0.001)
func_harmonic = harmonic(x)
noise_v = 0
y = ColumnDataSource(data = dict(x=x,f=func_harmonic))
plot = figure(title='y(t) = A ∗ sin(ω ∗ t + φ)):Original data',height=100,width=500,sizing_mode="scale_width")

plot.x_range = Range1d(-10,10)
plot.y_range = Range1d(-3,3)
plot.line(x='x',y='f',source=y)
#unfiltered data
plot2 = figure(title='Filtred data',height=100,width=500,sizing_mode="scale_width")
plot2.x_range = Range1d(-10,10)
plot2.y_range = Range1d(-3,3)

unfiltred_data = ColumnDataSource(data = dict(x=x,y=noise_filter(arr=(func_harmonic+noise_v))))
plot2.line(x='x',y='y',source=unfiltred_data)

plot2.visible = False

amplitude = Slider(start=-3, end=3, value=1,step=0.001,title="Amplitude")
frequency = Slider(start=-10, end=10, value=1,step=0.001,title="Frequency")
phase = Slider(start=-10, end=10, value=0, step=0.001, title="phase")
noise_mean_slider = Slider(start=-3, end=3, value=0, step=0.001, title="Noise mean slider")
noise_covariance_slider = Slider(start=0, end=3, value=0, step=0.001, title="Noise covariance slider")
check_b = CheckboxButtonGroup(labels=['Noise'])
k_slider = Slider(start=1, end=1000, value=1, step=2, title="K")


def update(attr,old,new):
    global func_harmonic
    global unfiltred_data
    if not check_b.active:
        func_harmonic = harmonic(x,amplitude=amplitude.value, frequency=frequency.value, phase=phase.value)
        y.data = {'x':x, 'f':func_harmonic}
    else:
        func_harmonic = harmonic(x,amplitude=amplitude.value, frequency=frequency.value, phase=phase.value)
        y.data = {'x':x, 'f':func_harmonic+noise_v}
        unfiltred_data.data = {'x':x, 'y':noise_filter(func_harmonic+noise_v)}


def check_box(new):
    global noise_v
    global unfiltred_data
    if check_b.active:
        plot2.visible = True
        noise_v = noise(noise_mean=noise_mean_slider.value, noise_covariance=noise_covariance_slider.value)
        y.data = {'x':x, 'f':func_harmonic+noise_v}
        unfiltred_data.data = {'x':x, 'y':noise_filter(func_harmonic+noise_v,k=k_slider.value)}
    else:
        noise_v = 0
        y.data = {'x':x, 'f':func_harmonic}
        plot2.visible = False
        
def update_noise(attr,old,new):
    if check_b.active:
        global noise_v
        global unfiltred_data
        noise_v = noise(noise_mean=noise_mean_slider.value, noise_covariance=noise_covariance_slider.value)
        y.data = {'x':x, 'f':func_harmonic+noise_v}
        unfiltred_data.data = {'x':x, 'y':noise_filter(func_harmonic+noise_v,k=k_slider.value)}


def k_slider_update(attr,old,new):
    global unfiltred_data
    if check_b.active:
        unfiltred_data.data = {'x':x, 'y':noise_filter(func_harmonic+noise_v,k=k_slider.value)}

check_b.on_event('button_click',check_box)
amplitude.on_change('value',update)
frequency.on_change('value',update)
phase.on_change('value',update)
noise_mean_slider.on_change('value',update_noise)
noise_covariance_slider.on_change('value',update_noise)
k_slider.on_change('value',k_slider_update)

button = Button(label="Reset", button_type="danger")

#unfiltred data
def reset():
    amplitude.value = 1
    frequency.value = 1
    phase.value = 0
    noise_mean_slider.value = 0
    noise_covariance_slider.value = 0
    k_slider.value = 1

button.on_click(reset)
curdoc().add_root((column([plot,plot2,amplitude,frequency,phase,check_b,noise_mean_slider,noise_covariance_slider,k_slider,button],sizing_mode="scale_width")))