import numpy as np
import cv2
import random
 
# https://www.labri.fr/perso/nrougier/teaching/numpy/numpy.html
# http://conwaylife.com/w/index.php?title=Pulsar
class Pulsar :
    def __init__(self):
        self.Z = np.zeros((22,22), dtype=int)
        # print width, height
        r1 = np.zeros(22, dtype=int)
        r2 = np.zeros(22, dtype=int)
        r1[4:7] = r1[10:13] = 1
        r2[2] = r2[7] = r2[9] = r2[14] = 1
        self.Z[2:8] = [r1,
                       np.zeros(22, dtype=int),
                       r2,
                       r2,
                       r2,
                       r1]
        self.Z[9:15] = self.Z[2:8][::-1]
        print self.Z[1:-1,1:-1]

    def iterate(self) :
        N = (self.Z[0:-2,0:-2] + self.Z[0:-2,1:-1] + self.Z[0:-2,2:] +
             self.Z[1:-1,0:-2]                     + self.Z[1:-1,2:] +
             self.Z[2:  ,0:-2] + self.Z[2:  ,1:-1] + self.Z[2:  ,2:])
        birth = (N==3) & (self.Z[1:-1,1:-1]==0)
        survive = ((N==2) | (N==3)) & (self.Z[1:-1,1:-1]==1)
        self.Z[...] = 0
        self.Z[1:-1,1:-1][birth | survive] = 1
        return self.Z[1:-1,1:-1]

def gol(p, rgbArray):
    cv2.rectangle(rgbArray, (0, 0), (rgbArray.shape[0]-1, rgbArray.shape[1]-1),
                  (0,0,0), -1)
    Z = p.iterate()
    grid = np.where(Z==1)
    grid = (grid[0]*20, grid[1]*20)
    r_offset = (random.randint(0,100),
                random.randint(0,100))
    r_color = (random.randint(0,255),
               random.randint(0,255),
               random.randint(0,255))
    for p in zip(grid[0], grid[1]):
        p= (p[0]+r_offset[0], p[1]+r_offset[1])
        cv2.rectangle(rgbArray, (p[1], p[0]),
                      (p[1]+19, p[0]+19),
                      r_color, -1)

def driver():
    rgbArray = np.zeros((400,400,3), 'uint8')
    p=Pulsar()
    for i in range(10):
        gol(p, rgbArray)
        cv2.imshow('image', rgbArray.astype(np.uint8))
        cv2.waitKey(0)
    
if __name__ == "__main__":
    driver()
