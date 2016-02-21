#!/usr/bin/env python
from xml.sax import ContentHandler, make_parser, InputSource
from xml.sax.handler import EntityResolver #feature_external_ges
import sys
import os.path

class Node:
    def __init__(self, name, parent = None, attr = None, start = 0):
        self._name = name
        self._start = start
        self._children = []
        self._parent = parent
        self._end = None
        self._attr = attr
    def getParent(self):
        return self._parent
    def addChild(self, child):
        self._children.append(child)
    def setEnd(self, end):
        #assert (self._end is None)
        self._end = end
    def getName(self):
        return self._name
    def getStart(self):
        return self._start
    def getEnd(self):
        return self._end
    def getPosition(self):
        if self._start == self._end:
            return "line %u" % self._start
        else:
            return "lines %u-%u" % (self._start, self._end)
    def getChildren(self):
        return self._children

    # Returns None if undefined
    def getAttribute(self, attr):
        try:
            return self._attr.getValue(attr)
        except KeyError:
            return None
    def getElementsByTagName(self, name):
        elements = []
        for child in self._children:
            if child.getName() == name:
                elements.append(child)
            subs = child.getElementsByTagName(name)
            if subs:
                elements.extend(subs)
        return elements
    def getValuesByAttrName(self, attr):
        values = []
        for child in self._children:
            val = child.getAttribute(attr)
            if val:
                values.append(val)
            val = child.getValuesByAttrName(attr)
            if val:
                values.extend(val)
        return values
    def getChildrenNames(self):
        return [n.getName() for n in self._children]

class LineHandler(ContentHandler):
    def __init__(self):
        ContentHandler.__init__(self)
        self._root = Node("root")
        self._current = self._root
    def setDocumentLocator(self, locator):
        self.locator = locator
    def startElement(self, name, attrs):
        node = Node(name, self._current, attrs, self.locator.getLineNumber())
        if self._current:
            self._current.addChild(node)
        else:
            print "Can't add node '%s': no parent!" % name
        self._current = node
    def endElement(self, name):
        #assert(self._current.getName() == name)
        self._current.setEnd(self.locator.getLineNumber())
        self._current = self._current.getParent()
    def endDocument(self):
        assert(self._current.getParent() == None)
        self._current.setEnd(self.locator.getLineNumber())
        self._current = None
    def getRoot(self):
        return self._root

class FixRelativeDTD(EntityResolver):
    def __init__(self, path):
        self._path = path
    def resolveEntity(self, publicId, systemId):
        if systemId:
            name = os.path.join(self._path, systemId)
            if os.path.isfile(name):
                source = InputSource()
                source.setByteStream(open(name, "rb"))
                return source
        # Using default resolution
        return EntityResolver.resolveEntity(self, publicId, systemId)
    
def report(node, msg):
    print "Object '%s' (%s): %s" % (node.getName(), node.getPosition(), msg)

def check_int(node, attr):
    val = node.getAttribute(attr)
    if not val:  report(node, "no '%s' defined" % attr)
    if not val.strip('.-+').isdigit(): report(node, "'%s' isn't a number (%s)" % (attr, val))

def check_if_int(node, attr):
    val = node.getAttribute(attr)
    if val and not val.strip('.-+').isdigit():
        report(node, "attribute '%s' is not a number (%s)" % (attr, val))
def check_if_bool(node, attr):
    val = node.getAttribute(attr)
    if val and val.lower() not in ("true", "false"):
        report(node, "attribute '%s' is not a boolean (%s)" % (attr, val))

if __name__ == "__main__":
    if len(sys.argv) != 2:
        print "Syntax: %s <xml document>" % sys.argv[0]
        sys.exit(0)
    base = os.path.dirname(sys.argv[1])
    
    handler = LineHandler()
    fix = FixRelativeDTD(base)
    parser = make_parser()
    parser.setContentHandler(handler)
    parser.setEntityResolver(fix)
    # Dirty hack that should avoid error on loading the DTD with a relative path
    #parser.setFeature(feature_external_ges, 0)
    source = open(sys.argv[1], "r")
    parser.parse(source)
    source.close()

    root = handler.getRoot().getChildren()[0]

    # Movement analysis
    movements = root.getElementsByTagName("movement")
    for mov in movements:         
        collisions = mov.getElementsByTagName("collision_rect")
        if len(collisions) != 1:
            report(mov, "no collision rectanle defined!")
        for col in collisions:
            for border in ("left", "right", "top", "bottom"):
                check_int(col, border)

        # Other attributes
        if not mov.getAttribute("name"):
            report(mov, "no name provided")
        check_int(mov, "speed")

        frames = mov.getElementsByTagName("frame")
        if not len(frames):
            report(mov, "no frame defined")
        else:
            for f in frames:
                members = f.getElementsByTagName("member")
                if not len(members):
                    report(f, "no member")
                else:
                    for m in members:
                        for param in ("dx", "dy", "follow_cursor_limit"):
                            check_if_int(m, param)
                        for param in ("follow_cursor"):
                            check_if_bool(m, param)

    # Alias analysis
    aliases = root.getElementsByTagName("alias")
    movements = root.getElementsByTagName("movement")

    alias_mov = root.getValuesByAttrName("movement")
    mov_names = root.getValuesByAttrName("name")

    for ref in aliases:
        name = ref.getAttribute("movement")

        # Check that correspond_to is matched
        corresp = ref.getAttribute("correspond_to")
        if corresp:
            if corresp not in mov_names and corresp not in alias_mov:
                report(ref, "no such correspondance '%s' for alias '%s'" % (corresp, name))

        # Check there's only one alias with a movement definition
        if alias_mov.count(name) > 1:
            report(ref, "found 2 aliases with same movement '%s'" % name)

    # Sprite analysis
    sprites=root.getElementsByTagName("sprite")

    # Sprite (type,name) not checked for duplicates now
    sprite_types=root.getValuesByAttrName("type")
    sprite_names=root.getValuesByAttrName("name")

    for sprite in sprites:
        if not sprite.getAttribute("name"):
            report(sprite, "no name provided")
        if not sprite.getAttribute("type"):
            report(sprite, "no type provided")

        # Check images
        images=sprite.getElementsByTagName("image")
        if len(images) != 1:
            report(sprite, "%u images used" % len(images))
        for img in images:
            fn=img.getAttribute("file")
            if fn:
                if not os.path.isfile(os.path.join(base, fn)):
                    report(sprite, "file %s doesn't exist" % fn)
            else:
                report(sprite, "no file defined!")

        # Check anchors
        anchors=sprite.getElementsByTagName("anchor")
        if len(anchors) != 1:
            report(sprite, "%u anchors defined" % len(anchors))

        # Check attached stuff
        for attached in sprite.getElementsByTagName("attached"):
            member_type = attached.getAttribute("member_type")
            if member_type:
                if member_type!="weapon" and not member_type in sprite_types:
                    report(attached, "no sprite to match member_type '%s'" % member_type)
            else:
                 report(attached, "no member_type provided")

            for offset in ("dx", "dy"):
                check_int(attached, offset)

    # Check clothes
    clothes = root.getElementsByTagName("clothe")
    #print "Clothes: %s" % [c.getAttribute("name") for c in clothes]
    for clothe in clothes:
        c_members = clothe.getElementsByTagName("c_member")
        if not len(c_members):
            report(clothe, "no c_member")
        else:
            for member in c_members:
                name = member.getAttribute("name")
                if name:
                    if name != "weapon" and name not in sprite_names:
                        report(member, "c_member of name '%s' has no associated sprite" % name)
                else:
                    report(member, "no name provided!")

    # Consistency check
    print "# Consistency check (ignoring movements whose name starts with 'weapon'"
    for mov in movements:
        A = mov.getAttribute("name")
        if not A or A.startswith("weapon"):
            continue

        # Finding cloth of such type
        foundc = None
        normal = None
        for clothe in clothes:
            if clothe.getAttribute("name") == A:
                foundc = clothe
            elif clothe.getAttribute("name") == "normal":
                normal = clothe
        if not foundc:
            if not normal:
                report(mov, "no cloth found at all for name '%s'!" % A)
                continue
            foundc = normal
            report(mov, "no clothe for movement of name '%s', using 'normal' instead" % A)
            A = "normal"

        c_members = [node.getAttribute("name") for node in foundc.getElementsByTagName("c_member")]
        
        for member in mov.getElementsByTagName("member"):
            B = member.getAttribute("type")
            if not B or B == 'weapon':
                continue

            found = 0
            for sprite in sprites:
                if sprite.getAttribute("type") == B:
                    if sprite.getAttribute("name") in c_members:
                        found += 1

            if not found:
                report(member, "found no c_member in clothe '%s' for any sprite of type '%s'!" % (A, B))
            if found > 1:
                report(member, "found %u sprites for which a c_member of type '%s' exists in clothe '%s'!" % (found, B, A))
