#!/usr/bin/python
import pdb, pygame, sys, math
from xml.sax import saxutils
from xml.sax import make_parser
from xml.sax.handler import feature_namespaces
from time import sleep

class Point:
	def __init__(self):
		self.x = 0
		self.y = 0

class MemberMvt:
	def __init__(self):
		self.dx = 0
		self.dy = 0
		self.angle = 0
		self.scale = Point()
		self.alpha = 0
		self.type = ''


class LoadClothe(saxutils.DefaultHandler):
	def __init__(self, name ):
		self.clothe_name = name
		self.current = False
		self.member_lst = []

	def startElement(self, tag, attrs):
		if tag == 'clothe':
			if attrs.get('name', None) != self.clothe_name: return
			self.current = True
			print "Clothe", self.clothe_name, "found!"

		if tag == 'member' and self.current == True:
			if attrs.get('name', None) != 'weapon':
				self.member_lst.append(attrs.get('name', None))

	def endElement(self, tag):
		if tag == 'clothe':
			self.current = False

	def GetMemberList(self):
		return self.member_lst

class LoadSprite(saxutils.DefaultHandler):
	def __init__(self, name ):
		self.sprite_name = name
		self.current = False
		self.png = pygame.image
		self.loaded = False
		self.attach = {}
		self.angle= 0
		self.x = 0
		self.y = 0
		self.x0 = 0
		self.y0 = 0
		self.dx = 0
		self.dy = 0

	def startElement(self, tag, attrs):
		if tag == 'sprite':
			if attrs.get('name', None) != self.sprite_name: return
			self.current = True
			self.type = attrs.get('type', None)

		if tag == 'image' and self.current == True:
			print 'Loading', attrs.get('file', None)
			self.png = pygame.image.load(attrs.get('file', None))
			self.rect = self.png.get_rect()
			self.loaded = True

		if tag == 'anchor' and self.current == True:
			self.dx = int(attrs.get('dx', None))
			self.dy = int(attrs.get('dy', None))
			self.loaded = True

		if tag == 'attached' and self.current == True:
			member = attrs.get('member_type', None)
			tmp = Point()
			tmp.x = int(attrs.get('dx', None))
			tmp.y = int(attrs.get('dy', None))
			self.attach[ member ] = tmp

	def endElement(self, tag):
		if tag == 'sprite':
			self.current = False

	def GetSprite(self):
		return self.png

	def Rotate(self):
		self.rot_png = pygame.transform.rotate(self.png, - self.angle * 180 / math.pi)
		rotation_point = Point()
		rotation_point.x = self.png.get_rect().width / 2 - self.rot_png.get_rect().width / 2
		rotation_point.y = self.png.get_rect().height / 2 - self.rot_png.get_rect().height / 2
		
		rhs_pos = Point()
		rhs_pos.x = self.dx
		rhs_pos.y = self.dy

		rhs_dst = math.sqrt((self.png.get_rect().width/2 - rhs_pos.x)*(self.png.get_rect().width/2 - rhs_pos.x)+(self.png.get_rect().height/2 - rhs_pos.y)*(self.png.get_rect().height/2 - rhs_pos.y))

		if rhs_dst == 0:
			rhs_angle = 0.0
		else:
			rhs_angle = - math.acos( float(rhs_pos.x - self.png.get_rect().width/2) / rhs_dst)


		if(self.png.get_rect().height/2 - rhs_pos.y < 0): rhs_angle = -rhs_angle

		rhs_angle += self.angle

		rotation_point.x -= self.png.get_rect().width / 2 + math.cos(rhs_angle)*rhs_dst
		rotation_point.y -= self.png.get_rect().height / 2 + math.sin(rhs_angle)*rhs_dst

		rotation_point.x += rhs_pos.x
		rotation_point.y += rhs_pos.y
		self.r_x = rotation_point.x
		self.r_y = rotation_point.y

class LoadMovement(saxutils.DefaultHandler):
	def __init__(self, name ):
		self.mvt_name = name
		self.current = False
		self.frame = []

	def startElement(self, tag, attrs):
		if tag == 'movement':
			if attrs.get('name', None) != self.mvt_name: return
			self.current = True

		if tag == 'frame' and self.current == True:
			self.frame.append( {} )

		if tag == 'member' and self.current == True:
			current = len(self.frame) - 1
			type = attrs.get('type', None)
			self.frame[current][type] = MemberMvt()
			self.frame[current][type].dx = int(attrs.get('dx', 0))
			self.frame[current][type].dy = int(attrs.get('dy', 0))
			self.frame[current][type].angle = float(attrs.get('angle', 0)) * math.pi / 180.0

	def endElement(self, tag):
		if tag == 'movement':
			self.current = False

	def GetSprite(self):
		return self.png

def SpriteFromType(type):
	i = 0
	for sprite in sprite_lst:
		if sprite.type == type:
			return i
		i += 1

	return -1

def MovementFromType(type, frame):
	i = 0
	for member in movement.frame[frame]:
		if member == type:
			return i
		i += 1

	return -1

def Place(member, dx, dy):
	sprite = SpriteFromType(member)
	if sprite == -1: return
	sprite_lst[sprite].x0 += dx
	sprite_lst[sprite].y0 += dy
	sprite_lst[sprite].x0 -= sprite_lst[sprite].dx
	sprite_lst[sprite].y0 -= sprite_lst[sprite].dy
	for attach in sprite_lst[sprite].attach:
		child_dx = sprite_lst[sprite].x0 + sprite_lst[sprite].attach[attach].x
		child_dy = sprite_lst[sprite].y0 + sprite_lst[sprite].attach[attach].y
		Place(attach, child_dx, child_dy)

def SetFrame(member, frame, dx, dy, angle):
	sprite = SpriteFromType(member)
	if sprite == -1:
		return

	sprite_lst[sprite].x = sprite_lst[sprite].x0 
	sprite_lst[sprite].y = sprite_lst[sprite].y0
	sprite_lst[sprite].angle = angle
	sprite_lst[sprite].x += dx
	sprite_lst[sprite].y += dy

	mvt = MovementFromType(member, frame)
	if mvt != -1:
		sprite_lst[sprite].x += movement.frame[frame][member].dx
		sprite_lst[sprite].y += movement.frame[frame][member].dy
		sprite_lst[sprite].angle += movement.frame[frame][member].angle

	for attach in sprite_lst[sprite].attach:
		child_dx = dx
		child_dy = dy
		child_angle = angle

		if mvt != -1:
			child_dx += movement.frame[frame][member].dx
			child_dy += movement.frame[frame][member].dy
			child_angle += movement.frame[frame][member].angle

		######
		tmp_x = sprite_lst[sprite].attach[attach].x - sprite_lst[sprite].dx
		tmp_y = sprite_lst[sprite].attach[attach].y - sprite_lst[sprite].dy
		radius = math.sqrt((tmp_x*tmp_x)+(tmp_y*tmp_y))

		angle_init = 0
		if radius != 0:
			if tmp_x > 0:
				if tmp_y > 0:
					angle_init = math.acos( tmp_x / radius )
				else:
					angle_init = -math.acos( tmp_x / radius )

			else:
				if tmp_y > 0:
					angle_init = math.acos( tmp_x / radius )
				else:
					angle_init = math.pi + math.acos( -tmp_x / radius )

		if mvt != -1:
			#angle0 = angle_init + sprite_lst[sprite].angle
			angle0 = angle_init + sprite_lst[sprite].angle
			child_dx += radius * (math.cos(angle_init + angle + movement.frame[frame][member].angle ) - math.cos(angle_init + angle))
			child_dy += radius * (math.sin(angle_init + angle + movement.frame[frame][member].angle ) - math.sin(angle_init + angle))

		SetFrame(attach, frame, child_dx, child_dy,  child_angle)
		#sprite_lst[sprite].angle +

pygame.init()
screen = pygame.display.set_mode([128, 128])

parser = make_parser()
parser.setFeature(feature_namespaces, 0)
clothe = LoadClothe('animation2')
parser.setContentHandler(clothe)
parser.parse('config.xml')

parser = make_parser()
parser.setFeature(feature_namespaces, 0)
movement = LoadMovement('animation2')
parser.setContentHandler(movement)
parser.parse('config.xml')


sprite_lst = []

for member in clothe.GetMemberList():
	sprite = LoadSprite( member )
	parser.setContentHandler( sprite )
	parser.parse('config.xml')
	if sprite.loaded != True:
		print 'Unable to load member:', member
		continue

	sprite_lst.append(sprite)

Place('body', 64, 64)

frame = 0
while 1:
	for event in pygame.event.get():
		if event.type == pygame.QUIT: sys.exit()

	screen.fill([0, 0, 0])
	for sprite in sprite_lst:
		#pdb.set_trace()
		SetFrame('body', frame%len(movement.frame), 0, 0, 0)
		sprite.Rotate()
		rect = sprite.GetSprite().get_rect()
		rect.left = sprite.x + sprite.r_x
		rect.top = sprite.y + sprite.r_y
		screen.blit(sprite.rot_png, rect)

	frame += 1
	pygame.display.flip()
	sleep(0.1)


