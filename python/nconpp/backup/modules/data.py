import json
import pickle
from datetime import datetime


class pickleDataHandler:
    """A short class for data handling using pickle."""

    def __init__(self, pickleDataObject=None):
        self.datafile = pickleDataObject  # datafile object
        self.datetime = datetime.now()  # flag for current datetime

    def dump_data(self, data):
        """Dump data to the simulation datafile."""
        pickle.dump(data, self.datafile)

    def load_data(self):
        data = []
        with open(self.datafile, "rb") as f:
            while True:
                try:
                    data.append(pickle.load(f))
                except EOFError:
                    break
        return data


class jsonDataHandler:
    """A short class for data handling using json."""

    def __init__(self, jsonDataObject=None):
        self.datafile = jsonDataObject  # datafile object
        self.datetime = datetime.now()  # flag for current datetime

    def dump_data(self, data):
        """Dump data to the simulation datafile."""
        json.dump(data, self.datafile)

    def load_data(self):
        data = []
        with open(self.datafile, "rb") as f:
            while True:
                try:
                    data.append(json.load(f))
                except EOFError:
                    break
        return data
