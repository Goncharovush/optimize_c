# This module test libasa work and check correct work of binding
from __future__ import print_function
import numpy as np
import unittest
import matplotlib.pyplot as plt
from libasa import Server, HOST, PORT, \
    AsaSettings, SetSettings, IvCurve, GetIVCurve, MAX_NUM_POINTS
from ctypes import c_double, c_uint32, c_size_t


def set_r_cs(volt, cur):
    """
     This function set r_cs corresponding to max_current and max_voltage
    :param volt: max_voltage
    :param cur: max_current
    :return: r_cs
    """
    standart_volt = [1, 1.5, 2, 2.5, 3, 4, 4.5, 5, 6, 6.7, 7.5, 10]
    if volt == standart_volt[0]:
        standart_cur = [0.5, 1, 5, 10]
        r_cs_set = [2000, 1000, 200, 100]
    elif volt == standart_volt[1]:
        standart_cur = [5, 15]
        r_cs_set = [300, 100]
    elif volt == standart_volt[2]:
        standart_cur = [1, 2, 5, 10]
        r_cs_set = [2000, 1000, 390, 200]
    elif volt == standart_volt[3]:
        standart_cur = [25]
        r_cs_set = [100]
    elif volt == standart_volt[4]:
        standart_cur = [10, 15]
        r_cs_set = [300, 200]
    elif volt == standart_volt[5]:
        standart_cur = [2, 10]
        r_cs_set = [2000, 390]
    elif volt == standart_volt[6]:
        standart_cur = [15]
        r_cs_set = [300]
    elif volt == standart_volt[7]:
        standart_cur = [5, 25, 50]
        r_cs_set = [1000, 200, 100]
    elif volt == standart_volt[8]:
        standart_cur = [15]
        r_cs_set = [390]
    elif volt == standart_volt[9]:
        standart_cur = [10]
        r_cs_set = [665]
    elif volt == standart_volt[10]:
        standart_cur = [25, 75]
        r_cs_set = [300, 100]
    else:
        standart_cur = [5, 10, 25, 50, 90]
        r_cs_set = [2000, 1000, 390, 200, 100]
    return r_cs_set[standart_cur.index(cur)]


class TestStringMethods(unittest.TestCase):

    def test_not_empty(self):
        # Got something non-empty
        server = Server(HOST, PORT)
        settings = AsaSettings()
        settings.number_points = c_uint32(100)
        settings.number_charge_points = c_uint32(400)
        settings.measure_flags = c_uint32(0)
        settings.disc_freq_hz = c_double(1000)
        settings.probe_signal_frequency_hz = c_double(100)
        settings.voltage_ampl_v = c_double(2)
        settings.max_current_m_a = c_double(10)
        settings.debug_model_type = c_uint32(1)
        settings.debug_model_nominal = c_double(150)
        settings.trigger_mode = c_uint32(0)
        SetSettings(server, settings)
        iv_curve = IvCurve()
        GetIVCurve(server, iv_curve, settings.number_points)
        currents = iv_curve.currents[:settings.number_points]
        voltages = iv_curve.voltages[:settings.number_points]
        self.assertTrue((sum(currents) != 0) and (sum(voltages) != 0))
        time = np.linspace(0., 1 / settings.disc_freq_hz, settings.number_points)
        # plot ivc if test ok
        plt.figure(figsize=(8, 5))
        plt.clf()
        plt.subplot(111)
        plt.title('Voltage, Current')
        plt.plot(time * 1000, currents, label='I')
        plt.plot(time * 1000, voltages, label='U')
        plt.xlabel('time, ms')
        plt.ylabel('U, I')
        plt.legend()
        plt.show()

    def test_iv_form(self):
        server = Server(HOST, PORT)
        settings = AsaSettings()
        settings.disc_freq_hz = c_double(1000)
        settings.number_points = c_uint32(100)
        settings.number_charge_points = c_uint32(400)
        settings.measure_flags = c_uint32(0)
        settings.probe_signal_frequency_hz = c_double(100)
        settings.voltage_ampl_v = c_double(2)
        settings.max_current_m_a = c_double(10)
        settings.debug_model_type = c_uint32(1)
        settings.debug_model_nominal = c_double(150)
        settings.trigger_mode = c_uint32(0)
        SetSettings(server, settings)
        iv_curve = IvCurve()
        GetIVCurve(server, iv_curve, settings.number_points)
        currents = iv_curve.currents[:]
        voltages = iv_curve.voltages[:]
        time = np.linspace(0., 1 / settings.disc_freq_hz,
                           settings.number_points)
        r = settings.debug_model_nominal
        r_cs = set_r_cs(settings.voltage_ampl_v, settings.max_current_m_a)
        # Check sin zero
        self.assertTrue(np.abs(currents[len(time) // 2]) <
                        settings.max_current_m_a / 20)
        # Check sin max
        self.assertTrue(np.abs(voltages[len(time) // 4]) >
                        settings.voltage_ampl_v * r / (2 * (r + r_cs)))


if __name__ == '__main__':
    unittest.main()
