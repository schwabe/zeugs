#! /usr/bin/python
import httplib
import os.path
from pprint import pprint
import StringIO

tiles = {}


#for i in range(-100,100):
#	for j in range(-100,100):
#		tiles[(i,j)] = None



#	http://imgs.xkcd.com/clickdrag/${i}n${j}w.png
h = httplib.HTTPConnection('imgs.xkcd.com')

if not os.path.exists("img"):
	os.mkdir("img")

os.chdir("img") 


def getimg(i,j):
	a = "e"
	b = "s"
	
	if i <= 0:
		a = "w"
		i = -i +1

	if j <= 0:
		b = "n"
		j = -j +1

	imgname = "%d%s%d%s.png" % (j,b,i,a)

	if (os.path.exists(imgname)):
		return imgname
	elif os.path.exists(imgname + ".404"):
		return None
	else:
		h.request("GET","http://imgs.xkcd.com/clickdrag/" + imgname)
		r = h.getresponse()
		print imgname, `r.reason,r.status`

		if r.status == 404:
			f =open(imgname + ".404","w")
			f.write(r.reason)
			f.write(r.read())
			f.close()
			return None
		else:
			data = r.read()
			f =open(imgname,"w")
			f.write(data)
			f.close()
			
			return imgname


#tiles[(0,0)]=getimg(0,0)
		
def getTile(i,j):
	tiles[(i,j)]=getimg(i,j)
	if tiles[(i,j)] == None:
		return
	
	for ni,nj in ((i-1,j),(i+1,j),(i,j-1),(i,j+1)):
		if not (ni,nj) in tiles:
			getTile(ni,nj)

getTile(0,0)

minx = miny = maxx = maxy = 0
for x,y in tiles.items():
	if y:
		minx = min(x[0],minx)
		miny = min(x[1],miny)
		maxx = max(x[0],maxx)
		maxy = max(x[1],maxy)


print minx , miny , maxx , maxy

tilesx, tilesy = (maxx -minx+1, maxy -miny+1)

imgx,imgy= tilesx* 2048, tilesy*2048

print imgx,imgy


if False:
	extraimg = []
	for i in range(-32,49):
		for j in range(-7,19):
			if not (i,j) in tiles:
				tiles[(i,j)]=getimg(i,j)

				if tiles[(i,j)]:
					extraimg.append((i,j))

	print extraimg
else:
	extraimg = [(-8, 2), (-6, -6), (-4, -5), (1, -5), (6, -7), (10, -2), (11, 11), (12, -4), (18, -2)]
	for x in extraimg:
		tiles[x]=getimg(x[0],x[1])
	
import Image

bigimg = Image.new('L',(imgx,imgy))
bigimg.paste(128,(0,0,imgx,imgy))


for x,y in tiles.items():
	if y:
		topx = (x[0] -minx) * 2048 
		topy = (x[1] - miny) * 2048 
		print "P",x
		smallimg = Image.open(y)
		smallimg.load()
		print smallimg.mode, bigimg.mode

		print (topx,topy,topx+2048,topy+2048)
		print smallimg.size
		bigimg.paste(smallimg, (topx,topy))

os.chdir("..")

bigimg.save("big.png")
big2 = bigimg.resize((imgx/3,imgy/3))
print "resize"
big2.save("big2.png")

big3 = bigimg.resize((imgx/10,imgy/10))
print "resize"
big3.save("big3.png")

