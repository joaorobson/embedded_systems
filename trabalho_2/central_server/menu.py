# Code based on: https://stackoverflow.com/questions/14200721/how-to-create-a-menu-and-submenus-in-python-curses

import curses
from curses import panel
import random
import threading
import json

global ref_temp
ref_temp = ""


class MenuTemplate():
    def __init__(self, items, stdscreen, server):
        self.window = stdscreen.subwin(0, 0)
        self.window.keypad(1)
        self.window.nodelay(1)
        self.panel = panel.new_panel(self.window)
        self.panel.hide()
        panel.update_panels()

        self.position = 0
        self.items = items
        self.server = server

    def navigate(self, n):
        self.position += n
        if self.position < 0:
            self.position = 0
        elif self.position >= len(self.items):
            self.position = len(self.items) - 1

    def display(self):
        pass


class MenuHandler(MenuTemplate):
    def __init__(self, items, stdscreen, server):
        MenuTemplate.__init__(self, items, stdscreen, server)

    def navigate(self, n):
        super().navigate(n)

    def check_for_alert(self, json_data, alert):
        alert_json = json_data.get("Alert")
        if alert == 0 and alert_json == 1:
            alert = 1
            self.server.control_alarm(alert)
            self.server.save_log({"Alert": alert})
        elif alert == 1 and alert_json == 0:
            alert = 0
            self.server.control_alarm(alert)
            self.server.save_log({"Alert": alert})

        if alert == 1:
            alert_msg = "ALERTA!!!!!"
        elif alert == 0:
            alert_msg = ""
    
        return alert, alert_msg

    def get_devices_current_state(self, devices, data):
        devices_keys = ["Lamp1", "Lamp2", "Lamp3", "Lamp4",
                        "AirC1","AirC2"]
        if not "Alert" in data and data:
            return {key:data[key] for key in data if key in devices_keys}
        return devices

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
                    msg = self.server.get_state_message(data[key], key)
                    if "Sens" in key:
                        color = curses.color_pair(4)
                    elif "Lamp" in key:
                        color = curses.color_pair(3)
                    elif "Air" in key:
                        color = curses.color_pair(2)
                elif key == "Temp":
                    msg = "{} °C".format(data[key])
                    color = curses.color_pair(5)
                elif key == "Hum":
                    msg = "{} %".format(data[key])
                    color = curses.color_pair(5)
                try:
                    self.window.addstr(index + 2, 0,
                                       "{}: {}".format(name, msg), color)
                    self.window.clrtoeol()
                except curses.error:
                    pass

    def init_term_colors(self):
        curses.start_color()
        curses.init_pair(1, curses.COLOR_RED, curses.COLOR_BLACK)
        curses.init_pair(2, curses.COLOR_BLUE, curses.COLOR_BLACK)
        curses.init_pair(3, curses.COLOR_GREEN, curses.COLOR_BLACK)
        curses.init_pair(4, curses.COLOR_YELLOW, curses.COLOR_BLACK)
        curses.init_pair(5, curses.COLOR_MAGENTA, curses.COLOR_BLACK)

    def display(self):
        self.panel.top()
        self.panel.show()
        self.window.clear()

        self.init_term_colors()

        alert = 0 
        alert_msg = ""
        devices_state = {}
        global ref_temp

        while True:
            self.window.refresh()
            curses.doupdate()

            json_data = self.server.json_data.copy()
            if "Alert" in json_data:
                alert, alert_msg = self.check_for_alert(json_data, alert)

            self.window.addstr(0,0, alert_msg, curses.color_pair(1))
            self.window.clrtoeol()

            self.show_devices_info(json_data)
            devices_state = self.get_devices_current_state(devices_state, json_data)

            self.window.addstr(1,0,"Temperatura de ref: {}".format(ref_temp))

            for index, item in enumerate(self.items):
                if index == self.position:
                    mode = curses.A_REVERSE
                else:
                    mode = curses.A_NORMAL

                msg = "%d. %s" % (index, item[0])
                self.window.addstr(1 + index, 40, msg, mode)

            key = self.window.getch()

            if key in [curses.KEY_ENTER, ord("\n")]:
                if self.position == len(self.items) - 2: # Selecionar Temperatura
                    self.items[self.position][1]()
                elif self.position == len(self.items) - 1: # Sair
                    break
                else:
                    device = self.items[self.position][1]
                    self.server.send('{{"Device": "{}"}}'.format(device))

                    self.server.save_log({"Device": (device, 1 - devices_state[device])})

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

class SubMenu(MenuTemplate):
    def __init__(self, items, stdscreen, server):
        MenuTemplate.__init__(self, items, stdscreen, server)

    def navigate(self, n):
        super().navigate(n)

    def display(self):
        self.panel.top()
        self.panel.show()
        self.window.clear()
        curses.curs_set(0)

        global ref_temp

        while True:
            self.window.refresh()
            curses.doupdate()

            for index, item in enumerate(self.items):
                if index == self.position:
                    mode = curses.A_REVERSE
                else:
                    mode = curses.A_NORMAL

                if index != 0:
                    msg = "%d. %s" % (index, item[0])
                else:
                    msg = "%d. %s: %s" % (index, item[0], ref_temp)
                self.window.addstr(1 + index, 0, msg, mode)
                self.window.clrtoeol()

            c = self.window.getch()

            if c != curses.ERR:
                if c in [curses.KEY_ENTER, ord("\n")]:
                    if self.position == len(self.items) - 1:
                        break
                    else:
                        self.items[self.position][1]()
                if c == curses.KEY_UP:
                    self.navigate(-1)
                elif c == curses.KEY_DOWN:
                    self.navigate(1)
                elif c == curses.KEY_BACKSPACE:
                    ref_temp = ref_temp[:-1]
                elif chr(c).isdigit() or chr(c) == ".":
                    ref_temp += chr(c)


        self.window.clear()
        self.panel.hide()
        panel.update_panels()
        curses.doupdate()


class Menu(object):
    def __init__(self, stdscreen, server):
        self.screen = stdscreen
        curses.curs_set(0)

        submenu_items = [("Nova temperatura", curses.beep), ("Salvar temperatura", "")]
        submenu = SubMenu(submenu_items, self.screen, server)


        main_menu_items = [
            ("Ligar/Desligar Lâmpada da cozinha", "Lamp1"),
            ("Ligar/Desligar Lâmpada da sala", "Lamp2"),
            ("Ligar/Desligar Lâmpada do quarto 1", "Lamp3"),
            ("Ligar/Desligar Lâmpada do quarto 2", "Lamp4"),
            ("Ligar/Desligar Ar Cond. do quarto 1", "AirC1"),
            ("Ligar/Desligar Ar Cond. do quarto 2", "AirC2"),
            ("Selecionar Temperatura", submenu.display),
            ("Sair", "Sair"),
        ]
        main_menu = MenuHandler(main_menu_items, self.screen, server)
        main_menu.display()
