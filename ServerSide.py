import serial
import MySQLdb
from time import sleep

debug_mode = True

if debug_mode:
	lista = ['XX XX XX XX', '34 76 B1 EB']
else:
	#Criando conexao com o banco
	con = MySQLdb.connect(host="45.77.161.79", user="root", passwd="teste123", db="lista")
	#obtendo cursor
	cursor = con.cursor()
	#Executando download da lista
	cursor.execute("SELECT * FROM lista")
	lista = []
	#Extraindo a lista
	for row in cursor.fetchall():
		lista.append(row[0])

# Funcao de busca da chave na lista
def checar(chave):
	global lista
	for k in lista:
		if(chave == k):
			return True
	return False

porta = '/dev/ttyACM0' # Porta comum no raspberry
baud_rate = 9600 # Frequencia de comunicacao

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