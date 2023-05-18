#!/usr/bin/env python
# Open TAS - A Command line interface for the Open TAS Controller.
# Copyright (C) 2019  Russell Small
#
# This program is free software: you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation, either version 3 of the License, or
# (at your option) any later version.
#
# This program is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.
#
# You should have received a copy of the GNU General Public License
# along with this program.  If not, see <https://www.gnu.org/licenses/>.

from math import floor

PREFIX = {
	0xFC: "[DEBUG] ",
	0xFD: "[INFO]  ",
	0xFE: "[WARN]  ",
	0xFF: "[ERROR] "
}

class Movie:
	def __init__(self, system, game, controllers, author, description):
		self.system = system
		self.game = game
		self.controllers = controllers
		self.author = author
		self.description = description

		self.frames = 0

	def play(self, connection):
		raise NotImplementedError()

	def record(self, connection):
		raise NotImplementedError()

	def write(self, raw, **kargs):
		raise NotImplementedError()

	def print(self):
		if self.system: print("System: ", self.system)
		if self.game: print("ROM:    ", self.game)
		if self.author: print("Author: ", self.author)
		if self.description: print("Desc:   ", self.description)

class N64Movie(Movie):
	def __init__(self, game, controllers, author, description):
		super().__init__("Nintendo 64", game, controllers, author, description)
		self.inputs = ([],) * controllers

	def play(self, connection, statusFunction = None):
		connection.write(bytearray([0x80])) #Set Device
		connection.write(b"N64")
		connection.write(bytearray([0x03])) #Datastream Playback Mode

		connection.write(bytearray([0xD1])) #Controller Config Raw Cmd
		connection.write(bytearray([0x01, 0x05, 0x00, 0x02]))
		connection.write(bytearray([0x00, 0x00, 0x00, 0x00]))
		connection.write(bytearray([0x00, 0x00, 0x00, 0x00]))
		connection.write(bytearray([0x00, 0x00, 0x00, 0x00]))

		frame = 0
		while True:
			command = connection.read(1)[0]
			if command in [0xFC, 0xFD, 0xFE, 0xFF]:
				data = connection.read_until(b"\n")[:-1]
				statusFunction(self, frame, None, PREFIX[command] + data.decode("utf-8")) if statusFunction else None
			elif command == 0xD0:
				fcount = floor(connection.read(1)[0]/4)
				data = b"".join(self.inputs[0][frame:frame + fcount])
				frame += fcount
				connection.write(bytearray([0xD0, len(data)]) + data)
				statusFunction(self, frame, data[-1]) if statusFunction else None
			else:
				print("Unknown Command: " + bytearray([command]).hex())


		# for frame in range(self.frames):
		# 	value = connection.read(1)[0]
		# 	inputs = [input[frame] for input in self.inputs]
		# 	connection.write(bytearray([0x01]) + b"".join(inputs))
		# 	statusFunction(self, frame, inputs) if statusFunction else None

	def record(self, connection, statusFunction = None):
		connection.write(bytearray([0x80])) #Set Device
		connection.write(b"N64")
		connection.write(bytearray([0x01])) #Record Mode

		print("Got it?")

		try:
			while True:
				command = connection.read(1)[0]	
				if command in [0xFC, 0xFD, 0xFE, 0xFF]:
					data = connection.read_until(b"\n")[:-1]
					statusFunction(self, message = PREFIX[command] + data.decode("utf-8")) if statusFunction else None
				elif command == 0xB0:
					(port, size, request_size) = connection.read(3)
					request = connection.read(request_size)
					response = connection.read(size - request_size)

					print(" ".join([
						bytearray([port]).hex(),
						bytearray([size]).hex(),
						bytearray([request_size]).hex(),
						request.hex(),
						response.hex()
					]))
				else:
					print("Unknown Command: " + bytearray([command]).hex())


		except KeyboardInterrupt:
			pass


	def write(self, raw, **kargs):
		for x in range(self.controllers):
			self.inputs[x].append(raw[x])
		self.frames += 1
