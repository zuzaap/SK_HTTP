from tkinter import *
from tkinter import ttk
from tkinter import filedialog
import re
import client
from tkinter import messagebox

response = ''

class AppStart(Tk):

    def __init__(self, *args, **kwargs):
        Tk.__init__(self, *args, **kwargs)
        containter = Frame(self)
        containter.pack(side=TOP, fill="both")

        self.frames = {}
        for F in (Start, Response):
            page_name = F.__name__
            frame = F(parent=containter, controller=self)
            self.frames[page_name] = frame
            frame.grid(row=0, column=0)
        self.show_frame("Start")

    def show_frame(self, page_name):
        frame = self.frames[page_name]
        frame.tkraise()


class Start(Frame):
    def __init__(self, parent, controller):
        Frame.__init__(self, parent)
        self.controller = controller

        self.kventrylist = []
        self.i = 1
        self.img_b = []
        self.text = None
        self.action = None

        title1 = Label(self, text="Podaj link:   http://")
        title1.grid(column=0, row=0)
        self.http = Entry(self)
        self.http.grid(column=1, row=0)

        title2 = Label(self, text="Wybierz, co chcesz zrobić:")
        title2.grid(column=0, row=1)

        actions = StringVar()
        self.combobox = ttk.Combobox(self, width=15, textvariable=actions)
        self.combobox['values'] = ("PUT", "DELETE", "GET", "HEAD")
        self.combobox.grid(column=1, row=1)

        combo_button = Button(self, text="Confirm", command=self.bttn_pressed)
        combo_button.grid(column=2, row=1)

        self.combobox.bind("<<ComboboxSelected>>", self.callbackfunc)
        con_bttn = Button(self, text="+", command=self.addNewElemnent)
        con_bttn.grid(column=2, row=2)

    def send_to(self, kvlist, http_link):
        global action
        global img_b
        http_linkk = re.search("/.*[A-Za-z0-9]*$", http_link)
        first_slash = http_link.find('/')
        first_colon = http_link.find(':')
        if first_colon == -1:
            messagebox.showerror("Error", "Need to add colon before port number")

        address = http_link[:first_colon]
        port = http_link[first_colon + 1: first_slash]

        print(address, port)

        msg = ''
        msg = msg + action + ' ' + http_linkk[0] + ' ' + 'HTTP/1.1' + '\r\n'

        for [k, v] in kvlist:
            msg = msg + k + ': ' + v + '\r\n'

        body = False
        if (len(self.img_b) != 0):
            msg = msg + 'Content-Length: ' + str(len(self.img_b))
            msg = msg + '\r\n\r\n' + str(self.img_b)
            body = True

        if not body:
            msg = msg + '\r\n'

        port = int(port)

        return address, port, msg

    def callbackfunc(self, event):
        global action
        global img_b
        action = self.combobox.get()
        if action == 'PUT':
            self.file = filedialog.askopenfilename(initialdir="D:\Informatyka\semestr_5\SK_PROJECT",
                                                   title="Select file",
                                                   filetypes=(("text files", "*.txt"),("html files", "*.html")))
            image = str(self.file)
            with open(image, 'r') as img:
                self.img_b = img.read()
            print(self.img_b)

            print(self.file)

    def bttn_pressed(self):
        global response
        kvlist = []
        for [key, value] in self.kventrylist:
            ks = key.get()
            vs = value.get()
            kvlist.append([ks, vs])
        http_link = self.http.get()
        address, port, msg = self.send_to(kvlist, http_link)
        text = client.client(address, port, msg)
        response = text
        self.controller.show_frame("Response")


    def addNewElemnent(self):
        global i
        global kventrylist
        self.i = self.i + 1
        key = Entry(self)
        key.grid(column=0, row=self.i)
        value = Entry(self)
        value.grid(column=1, row=self.i)
        self.kventrylist.append([key, value])
        self.i = self.i + 1


class Response(Frame):
    def __init__(self, parent, controller):
        global response

        def clear():
            show_txt.delete('1.0', END)
            show_txt.insert(END, response)

        Frame.__init__(self, parent)
        self.controller = controller
        window = Tk()
        rec = Label(window, text="Odebrane dane")
        rec.pack(side=TOP)
        show = Text(window)
        cl = Button(window, text = "Refresh", command = clear)
        cl.pack(side = TOP)

        show_txt = Text(window)
        show_txt.insert(END, response)
        show_txt.pack(side=LEFT)


if __name__ == "__main__":
    app = AppStart()
    app.mainloop()
