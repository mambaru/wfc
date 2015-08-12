import random
import string

counter = 0 

def genstr():
  global counter
  rndstr = ''.join(random.choice(string.ascii_uppercase) for _ in range(random.randint(1,20)) )
  result = '{0}-{1}'.format(counter, rndstr)
  counter += 1  
  return result
