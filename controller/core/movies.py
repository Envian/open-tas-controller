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
		#connection.write("c".encode()) # begin playback
		#connection.write("n64".encode()) # Nintendo 64
		#connection.write(bytearray([self.controllers]))
		#connection.write(bytearray([0x81, 0x04])) # Temp - Set Controller packet size.
		connection.write(bytearray([0x00])) #Connected byte (temp)
		connection.write(bytearray([0x81])) #Controller Config Raw Cmd
		connection.write(bytearray([0x01, 0x05, 0x00, 0x02]))
		connection.write(bytearray([0x00, 0x00, 0x00, 0x00]))
		connection.write(bytearray([0x00, 0x00, 0x00, 0x00]))
		connection.write(bytearray([0x00, 0x00, 0x00, 0x00]))

		frame = 0
		while True:
			command = connection.read(1)[0]
			if command == 0xff:
				# This is a debug message instead.
				print(connection.read_until(b"\n"))
				continue
			if command == 0xfe:
				# This is a debug message instead.
				print(connection.read_until(b"\n")[:-1].hex())
				continue 

			fcount = floor(connection.read(1)[0]/4)
			data = b"".join(self.inputs[0][frame:frame + fcount])
			frame += fcount
			connection.write(bytearray([0x80, len(data)]) + data)
			statusFunction(self, frame, data[-1]) if statusFunction else None


		# for frame in range(self.frames):
		# 	value = connection.read(1)[0]
		# 	inputs = [input[frame] for input in self.inputs]
		# 	connection.write(bytearray([0x01]) + b"".join(inputs))
		# 	statusFunction(self, frame, inputs) if statusFunction else None

	def record(self, connection, statusFunction = None):
		connection.write(bytearray([0x0B])) # begin Recording
		connection.write(bytearray([0x40])) # Nintendo 64
		connection.write(bytearray([self.controllers]))

		try:
			while True:
				inputs = [connection.read(4) for x in range(self.controllers)]
				self.write(b"".join(inputs))
				statusFunction(self, frame, inputs) if statusFunction else None


		except KeyboardInterrupt:
			pass


	def write(self, raw, **kargs):
		for x in range(self.controllers):
			self.inputs[x].append(raw[x])
		self.frames += 1
