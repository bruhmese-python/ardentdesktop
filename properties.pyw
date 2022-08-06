import time
import sys
import subprocess as sp
try:
    import tkinter as tk
    from tkinter import Tk
    from tkinter.constants import *
except ImportError:  # Python 2.x
    import Tkinter as tk
    from Tkinter import Tk
    from Tkconstants import *


def get_command(path: str):
    cmd = 'wmic datafile where Name="{}" list /format:list'.format(
        path.replace('\\', '\\\\'))
    return cmd


def StartMove(root, event):
    root.x = event.x
    root.y = event.y


def StopMove(root, event):
    root.x = None
    root.y = None


def OnMotion(root, event):
    deltax = event.x - root.x
    deltay = event.y - root.y
    x = root.winfo_x() + deltax
    y = root.winfo_y() + deltay
    root.geometry("+%s+%s" % (x, y))


def dialog(contents: str):
    root = Tk()
    root.overrideredirect(True)
    ws = root.winfo_screenwidth()
    hs = root.winfo_screenheight()

    gif1 = tk.PhotoImage(file='res/properties_bg.png')
    WIN_W, WIN_H = gif1.width(), gif1.height()

    x = (ws) * (0.5) - (WIN_W / 2)
    y = (hs / 2) - (WIN_H / 2)

    root.geometry('%dx%d+%d+%d' % (WIN_W, WIN_H, x, y))

    canvas = tk.Canvas(width=WIN_W, height=WIN_H, bg='#000000',
                       relief='ridge', highlightthickness=0)
    canvas.pack(expand=YES, fill=BOTH)
    canvas.create_image(0, 0, image=gif1, anchor=NW)

    text_area = canvas.create_text(200, 300, fill="white", font="Arial 8",
                                   text=contents)

    canvas.bind("<ButtonPress-1>",
                lambda event: StartMove(root, event))
    canvas.bind("<ButtonPress-3>", lambda event: exit())
    canvas.bind("<ButtonRelease-1>",
                lambda event: StopMove(root, event))
    canvas.bind("<B1-Motion>", lambda event: OnMotion(root, event))
    root.attributes("-transparentcolor", '#000000')
    root.attributes('-topmost', True)

    tk.mainloop()


if __name__ == "__main__":
    if sys.argv.__len__() < 2:
        exit()
        pass
    else:
        output = sp.getoutput(get_command(sys.argv[1])).replace(
            '\n\n', '\n').replace("=", " : ")
        dialog(output)
