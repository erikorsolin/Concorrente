from time import sleep
from random import randint
from threading import Thread, Semaphore

def produtor():
    global buffer
    for i in range(10):
        sleep(randint(0,2))  # fica um tempo produzindo...
        item = 'item ' + str(i)
        espacos_vazios.acquire()  # decrementa o contador de espaços vazios
        mutex.acquire()  # entra na região crítica
        buffer.append(item)
        print('Produzido %s (ha %i itens no buffer)' % (item, len(buffer)))
        mutex.release()  # sai da região crítica
        itens_disponiveis.release()  # incrementa o contador de itens disponíveis

def consumidor():
    global buffer
    for i in range(10):
        itens_disponiveis.acquire()  # decrementa o contador de itens disponíveis
        mutex.acquire()  # entra na região crítica
        item = buffer.pop(0)
        print('Consumido %s (ha %i itens no buffer)' % (item, len(buffer)))
        mutex.release()  # sai da região crítica
        espacos_vazios.release()  # incrementa o contador de espaços vazios
        sleep(randint(0,2))  # fica um tempo consumindo...

buffer = []
tam_buffer = 3

# cria semáforos
espacos_vazios = Semaphore(tam_buffer)  # inicializa com o número total de espaços vazios
itens_disponiveis = Semaphore(0)  # inicializa com zero itens disponíveis
mutex = Semaphore(1)  # semáforo binário para acesso exclusivo ao buffer

produtor = Thread(target=produtor)
consumidor = Thread(target=consumidor)
produtor.start()
consumidor.start()
produtor.join()
consumidor.join()
