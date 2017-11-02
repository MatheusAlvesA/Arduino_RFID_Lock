import serial

def checar(chave):
	if(chave == 'XX XX XX XX'):
		return True
	return False


porta = '/dev/ttyACM0'
baud_rate = 9600

porta = serial.Serial(porta, baud_rate)

if __name__=='__main__':
 
    print("===========================================")
    print("==========   Programa iniciado   ==========")
    print("===========================================")

    while True:
	    ID = porta.readline()[:-2];
	    if(checar(ID)):
	    	porta.write('S') # chave autorizada
	    	print "Chave autorizada: [",ID,"]\n"
	    else:
	    	porta.write('N') # Acesso negado
	    	print "Chave negada: [", ID,"]\n"

porta.close()