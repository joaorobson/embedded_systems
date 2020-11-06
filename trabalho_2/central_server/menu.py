# Code based on: https://stackoverflow.com/questions/14200721/how-to-create-a-menu-and-submenus-in-python-curses

import curses
from curses import panel
import random
import threading
import json

class MenuHandler(object):
    def __init__(self, items, stdscreen, server):
        self.window = stdscreen.subwin(0, 0)
        self.window.nodelay(1)
        self.window.keypad(1)
        self.panel = panel.new_panel(self.window)
        self.panel.hide()
        panel.update_panels()
        
        self.position = 0 
        self.items = items
        self.items.append(("exit", "exit"))

        self.server = server

    def navigate(self, n):
        self.position += n
        if self.position < 0:
            self.position = 0
        elif self.position >= len(self.items):
            self.position = len(self.items) - 1


    def check_for_alert(self, json_data, alert):

        alert_json = json_data.get("Alert", -1)
        alert_msg = ""
        if alert_json == 1:
            alert = 1
        elif alert_json == 0:
            alert = 0

        if alert == 1:
            alert_msg = "ALERTA!!!!!"
        elif alert == 0:
            alert_msg = ""
    
        return alert, alert_msg

    def switch_device_state(self, device_name):
        pass 

    def get_state_message(self, state, key):
        if key.startswith("Lamp"):
            if state == 1:
                return "Ligada"
            return "Desligada"

        elif key.startswith("AirC"):
            if state == 1:
                return "Ligado"
            return "Desligado"

        elif key.startswith("PresSens"):
            if state == 1:
                return "Ativado"
            return "Desativado"

        elif key.startswith("OpenSens"):
            if state == 1:
                return "Aberta"
            return "Fechada"


    def show_devices_info(self, data):
        if not "Alert" in data and data:
            devices_names = ["Temperatura", "Umidade",
                             "Lâmp. da Cozinha", "Lâmp. da Sala", "Lâmp. do Quarto 1",
                             "Lâmp. do Quarto 2", "Ar Cond. do Quarto 1", 
                             "Ar Cond. do Quarto 2", "Sens. Pres. da Sala",
                             "Sens. Pres. da Cozinha", "Porta da Cozinha",
                             "Janela da  Cozinha", "Porta da Sala", "Janela da Sala",
                             "Janela do Quarto 1", "Janela do Quarto 2"]

            devices_keys = ["Temp", "Hum", "Lamp1", "Lamp2", "Lamp3", "Lamp4",
                            "AirC1","AirC2","PresSens1", "PresSens2", 
                            "OpenSens1", "OpenSens2", "OpenSens3", "OpenSens4",
                            "OpenSens5", "OpenSens6"]
            for index, (key, name) in enumerate(zip(devices_keys, devices_names)):
                if key != "Temp" and key != "Hum":
                    msg = self.get_state_message(data[key], key)
                elif key == "Temp":
                    msg = "{} °C".format(data[key])
                elif key == "Hum":
                    msg = "{} %".format(data[key])

                self.window.addstr(index + 1, 0, 
                                   "{}: {}".format(name, msg))
                self.window.clrtoeol()
        

    def display(self):
        self.panel.top()
        self.panel.show()
        self.window.clear()
        curses.start_color()
        curses.init_pair(1, curses.COLOR_RED, curses.COLOR_BLACK)
        lampadas = [0, 0, 0]
        alert = -1
        alert_msg = ""
        while True:
            self.window.refresh()
            curses.doupdate()
            json_data = self.server.json_data.copy()
            alert, alert_msg = self.check_for_alert(json_data, alert)


            self.window.addstr(0,0, alert_msg, curses.color_pair(1))
            self.window.clrtoeol()

            self.show_devices_info(json_data)

            for index, item in enumerate(self.items):
                if index == self.position:
                    mode = curses.A_REVERSE
                else:
                    mode = curses.A_NORMAL

                msg = "%d. %s" % (index, item[0])
                self.window.addstr(1 + index, 40, msg, mode)

            key = self.window.getch()

            if key in [curses.KEY_ENTER, ord("\n")]:
                if self.position == len(self.items) - 1:
                    break
                else:
                    self.server.send('{{"Device": "{}"}}'.format(self.items[self.position][1]))

            elif key == curses.KEY_UP:
                self.navigate(-1)

            elif key == curses.KEY_DOWN:
                self.navigate(1)

        self.server.close()
        self.server.run_server = False
        self.window.clear()
        self.panel.hide()
        panel.update_panels()
        curses.doupdate()


class Menu(object):
    def __init__(self, stdscreen, server):
        self.screen = stdscreen
        curses.curs_set(0)

        main_menu_items = [
            ("Ligar/Desligar Lâmpada da cozinha", "Lamp1"),
            ("Ligar/Desligar Lâmpada da sala", "Lamp2"),
            ("Ligar/Desligar Lâmpada do quarto 1", "Lamp3"),
            ("Ligar/Desligar Lâmpada do quarto 2", "Lamp4"),
            ("Ligar/Desligar Ar Cond. do quarto 1 ", "AirC1"),
            ("Ligar/Desligar Ar Cond. do quarto 2", "AirC2"),
        ]
        main_menu = MenuHandler(main_menu_items, self.screen, server)
        main_menu.display()
