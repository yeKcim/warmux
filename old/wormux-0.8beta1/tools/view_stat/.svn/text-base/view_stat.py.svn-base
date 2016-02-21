#!/usr/bin/python
# -*- coding: UTF-8 -*-

import sys, os, types
import pygtk
pygtk.require ('2.0')
import gtk, gtk.glade
from xml.dom.minidom import parse

class GUI:
    def __init__(self, glade_xml):
        xml = gtk.glade.XML(glade_xml, "main_window")
        self.window = xml.get_widget('main_window')
        self.stats = xml.get_widget('stats')
        xml.signal_autoconnect(self)
        self.window.set_size_request(600,400)
        self.build_stats_widget()

    def createTreeviewColumns(self, treeview, colums):
        num = 0
        for name in colums:
            col = gtk.TreeViewColumn(name)
            treeview.append_column(col)
            cell = gtk.CellRendererText()
            col.pack_start(cell, True)
            col.add_attribute(cell, 'text', num)
            treeview.set_search_column(num)
            col.set_sort_column_id(num)
            num = num + 1
    
    def build_stats_widget(self):
        self.stats_store = gtk.TreeStore(str, str, int, int, int, int, int)
        self.stats.set_model(self.stats_store)
        self.createTreeviewColumns(self.stats, \
            ("Function", "Percent", "Count", "Total", "Average", "Min", "Max",))

    def addStat(self, parent, stat):
        return self.stats_store.append(parent, \
            (stat.function, "%.2f%%" % stat.percent, stat.count, \
             stat.total, stat.average, stat.min, stat.max,))

    def run(self):
        try:
            gtk.main()
        except KeyboardInterrupt:
            print "Interrupted (CTRL+C)."
        else:
            print "Quit."
            
    def on_main_window_destroy(self, widget):
        gtk.main_quit()

class StatItem:
    def __init__(self, function, percent, count, total, min, max):
        self.function = function
        self.percent = percent
        self.count = count
        self.total = total
        self.min = min
        self.max = max
        self.average = total/count
        self.tree = None

class StatTree:
    def __init__(self, name="root", parent=None):
        self.name = name
        self.parent = parent
        self.nodes = {}
        self.total = 0

    def addNode(self, node):
        self._addNode(node)
        
    def _addNode(self, node):
        index = node.function.split(":")
        if 1<len(index):
            parent = index[0]
            index = index[1:]
            node.function = ":".join(index)
            if parent not in self.nodes:
                subtree = StatItem(parent, 0, 1, 0, 0, 0)
                subtree.function = parent
                subtree.tree = StatTree(parent, self)
                self.nodes[parent] = subtree
            self.nodes[parent].tree._addNode(node)
        else:
            self.nodes[node.function] = node
        self.total = self.total + node.total

    def getFullname(self):
        t = ""
        s = self
        while s != None:
            if t != "":
                t = s.name + ">" + t
            else:
                t = s.name
            s = s.parent
        return t

    def __getitem__(self, index):
        return self.nodes[index]

class Stats:
    def __init__(self, filename):
        self.tree = StatTree() 
        self.parents = {}
        xml = parse(filename)
        root = xml.documentElement
        self.total_time = int(root.getAttribute("total_time"))
        self.description = root.getAttribute("description")

        # Create stat tree
        for node in root.childNodes:
            if node.nodeType == node.ELEMENT_NODE and node.tagName == "item":
                # Load on item
                total = int(node.getAttribute("total"))
                function = node.getAttribute("function")
                item = StatItem( \
                    function,
                    total * 100 / self.total_time,
                    int(node.getAttribute("count")),
                    total,
                    int(node.getAttribute("min")),
                    int(node.getAttribute("max")))

                # Insert item in the tree
                self.tree.addNode(item)

    def updateGUI(self, gui, tree=None, gui_parent=None, parents=[]):                    
        if tree==None:
            tree = self.tree
        for parent in tree.nodes:
            item = tree[parent]
            if item.tree != None:
                item.total = item.tree.total
                item.percent = item.total*100/tree.total
                new_gui_parent = gui.addStat(gui_parent, item)
                self.updateGUI(gui, item.tree, new_gui_parent)
            else:
                item.percent = item.total*100/tree.total
                gui.addStat(gui_parent, item)

def usage():
    print "Usage: %s file.xml" % sys.argv[0]
    sys.exit(1)

def main():
    if len(sys.argv) < 2:
        usage()
    filename = sys.argv[1]

    # Load GUI
    xml= os.path.join(os.path.dirname(__file__), "view_stat.glade")
    gui = GUI(xml)

    # Load stats
    stats = Stats(filename)
    stats.updateGUI(gui)

    # Run GUI
    gui.run()

if __name__=="__main__":
    main()
