# Code based on: https://stackoverflow.com/questions/14200721/how-to-create-a-menu-and-submenus-in-python-curses

import curses
from curses import panel
import random
import threading
import server
import json

class MenuHandler(object):
    def __init__(self, items, stdscreen):
        self.window = stdscreen.subwin(0, 0)
        self.window.nodelay(1)
        self.window.keypad(1)
        self.panel = panel.new_panel(self.window)
        self.panel.hide()
        panel.update_panels()
        
        self.position = 0
        self.items = items
        self.items.append(("exit", "exit"))

    def navigate(self, n):
        self.position += n
        if self.position < 0:
            self.position = 0
        elif self.position >= len(self.items):
            self.position = len(self.items) - 1

    def check_for_alert(self, json_data, alert):
        alert_json = json_data.get("alert", -1)
        alert_msg = ""
        if alert_json == 1:
            alert = 1
        elif alert_json == 0:
            alert = 0

        if alert == 1:
            alert_msg = "ALERTA"
        elif alert == 0:
            alert_msg = ""
    
        return alert, alert_msg

    def display(self):
        self.panel.top()
        self.panel.show()
        self.window.clear()
        lampadas = [0, 0, 0]
        alert = -1
        alert_msg = ""
        while True:
            json_data = json.loads(server.data)
            alert, alert_msg = self.check_for_alert(json_data, alert)
            self.window.refresh()
            curses.doupdate()

            self.window.addstr(2,0,str(json_data))
            self.window.addstr(1,0,str(lampadas[0]))
            self.window.addstr(0,0, str(alert_msg) + str(alert))
            self.window.clrtoeol()

            for index, item in enumerate(self.items):
                if index == self.position:
                    mode = curses.A_REVERSE
                else:
                    mode = curses.A_NORMAL

                msg = "%d. %s" % (index, item[0])
                self.window.addstr(7 + index, 1, msg, mode)

            key = self.window.getch()

            if key in [curses.KEY_ENTER, ord("\n")]:
                if self.position == len(self.items) - 1:
                    break
                else:
                    lampadas[self.position] = 1 - lampadas[self.position]
                    self.items[self.position][1]()

            elif key == curses.KEY_UP:
                self.navigate(-1)

            elif key == curses.KEY_DOWN:
                self.navigate(1)

        server.run_server = False
        self.window.clear()
        self.panel.hide()
        panel.update_panels()
        curses.doupdate()


class Menu(object):
    def __init__(self, stdscreen):
        self.screen = stdscreen
        curses.curs_set(0)

        main_menu_items = [
            ("Ligar lâmpada 1", curses.beep),
            ("Ligar lâmpada 2", curses.beep),
            ("Ligar lâmpada 3", curses.beep),
        ]
        main_menu = MenuHandler(main_menu_items, self.screen)
        main_menu.display()
