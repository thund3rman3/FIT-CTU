import socket
import time

def data_recieve_all(data):
    data = data.decode('ascii')
    return data


def data_send(data):
    data = str(data)
    data += '\a\b'
    data = data.encode('ascii')
    return data


def calc_hash(id_hash, key):
    hashkey = id_hash + key
    if hashkey >= 65536:
        final_hash = hashkey % 65536
    else:
        final_hash = hashkey
    return final_hash


def get_coordinates(robot_position,SERVER_SYNTAX_ERROR,connection):
    char_index = 0
    x, y = '', ''
    for i in robot_position:
        char_index += 1
        if i == ' ':
            for j in robot_position[char_index:]:
                char_index += 1
                if j == " ":
                    for k in robot_position[char_index:]:
                        y += k
                        if k==' ':
                            connection.sendall(SERVER_SYNTAX_ERROR)
                            print('close 38')#nevyskytuje se
                            return '0','0'
                    break
                else:
                    x += j
            break
    print(f'x={x}, y={y}')
    try:
        x,y=int(x), int(y)
    except:
        connection.sendall(SERVER_SYNTAX_ERROR)
        print('close 49')
        return '0','0'

    return x, y


def turn_around180(connection, SERVER_TURN_RIGHT, lenght,SERVER_SYNTAX_ERROR):
    connection.sendall(SERVER_TURN_RIGHT)
    robot_position = recieving_from_chunks(connection, lenght, SERVER_SYNTAX_ERROR)
    connection.sendall(SERVER_TURN_RIGHT)
    robot_position = recieving_from_chunks(connection, lenght, SERVER_SYNTAX_ERROR)
    return robot_position


def move_forward(connection, SERVER_MOVE, lenght, SERVER_SYNTAX_ERROR):
    connection.sendall(SERVER_MOVE)
    robot_position = recieving_from_chunks(connection, lenght, SERVER_SYNTAX_ERROR)
    return robot_position


def turn_left(connection, SERVER_TURN_LEFT, lenght, SERVER_SYNTAX_ERROR):
    connection.sendall(SERVER_TURN_LEFT)
    robot_position = recieving_from_chunks(connection, lenght, SERVER_SYNTAX_ERROR)
    return robot_position


def turn_right(connection, SERVER_TURN_RIGHT, lenght, SERVER_SYNTAX_ERROR):
    connection.sendall(SERVER_TURN_RIGHT)
    robot_position = recieving_from_chunks(connection, lenght, SERVER_SYNTAX_ERROR)
    return robot_position


def check_message(x2, y2, SERVER_PICK_UP, connection, CLIENT_MESSAGE, SERVER_LOGOUT, lenght, SERVER_SYNTAX_ERROR):
    if (x2, y2) == (0, 0):
        connection.sendall(SERVER_PICK_UP)
        CLIENT_MESSAGE = recieving_from_chunks(connection, lenght, SERVER_SYNTAX_ERROR)
        if CLIENT_MESSAGE == '0':
            return False
        print(CLIENT_MESSAGE)
        if CLIENT_MESSAGE:
            connection.sendall(SERVER_LOGOUT)
            return True
    else:
        return False

def DirectionLRLtoZero(connection, SERVER_TURN_LEFT, client_ok_lengh,SERVER_SYNTAX_ERROR,robot_position,SERVER_MOVE,SERVER_TURN_RIGHT, newX,newY):
                                    robot_position = turn_left(connection, SERVER_TURN_LEFT, client_ok_lengh,
                                                               SERVER_SYNTAX_ERROR)
                                    robot_position = move_forward(connection, SERVER_MOVE, client_ok_lengh,
                                                                  SERVER_SYNTAX_ERROR)
                                    robot_position = turn_right(connection, SERVER_TURN_RIGHT, client_ok_lengh,
                                                                SERVER_SYNTAX_ERROR)
                                    robot_position = move_forward(connection, SERVER_MOVE, client_ok_lengh,
                                                                  SERVER_SYNTAX_ERROR)
                                    robot_position = turn_left(connection, SERVER_TURN_LEFT, client_ok_lengh,
                                                               SERVER_SYNTAX_ERROR)
                                    prevX,prevY=newX,newY
                                    newX,newY =get_coordinates(robot_position,SERVER_SYNTAX_ERROR,connection)
                                    print('LRL')
                                    return robot_position,newX,newY,prevX,prevY

def DirectionRLRtoZero(connection, SERVER_TURN_LEFT, client_ok_lengh,SERVER_SYNTAX_ERROR,robot_position, SERVER_MOVE,SERVER_TURN_RIGHT, newX,newY):
                                    robot_position = turn_right(connection, SERVER_TURN_RIGHT, client_ok_lengh,
                                        SERVER_SYNTAX_ERROR)
                                    robot_position = move_forward(connection, SERVER_MOVE, client_ok_lengh,
                                                                  SERVER_SYNTAX_ERROR)
                                    robot_position = turn_left(connection, SERVER_TURN_LEFT, client_ok_lengh,
                                                               SERVER_SYNTAX_ERROR)
                                    robot_position = move_forward(connection, SERVER_MOVE, client_ok_lengh,
                                                                  SERVER_SYNTAX_ERROR)
                                    robot_position = turn_right(connection, SERVER_TURN_RIGHT, client_ok_lengh,
                                                                SERVER_SYNTAX_ERROR)
                                    prevX, prevY = newX, newY
                                    newX, newY = get_coordinates(robot_position, SERVER_SYNTAX_ERROR, connection)
                                    print('RLR')
                                    return robot_position,newX,newY,prevX,prevY


def recieving_from_chunks(connection, lenght,SERVER_SYNTAX_ERROR):
    b = bytes('\b', 'ascii').decode('ascii')
    a = bytes('\a', 'ascii').decode('ascii')
    data, recieved, outcome = "", "", ""
    while True:
        data = connection.recv(1)
        recieved = data_recieve_all(data)
        outcome += recieved
        if a in recieved:
            if len(outcome) > lenght-1 :
                #print(f'received a {outcome}')
                connection.sendall(SERVER_SYNTAX_ERROR)
                print('close 139')
                return '0'
            data = connection.recv(1)
            recieved = data_recieve_all(data)
            outcome += recieved
            if b in recieved:
                outcome = outcome[0:-2]
                #print(f'received ab {outcome}')
                break

    return outcome


def get_around_obstacle(connection, SERVER_MOVE, SERVER_TURN_RIGHT, SERVER_TURN_LEFT, client_ok_lengh, SERVER_SYNTAX_ERROR):
    robot_position = turn_left(connection, SERVER_TURN_LEFT, client_ok_lengh, SERVER_SYNTAX_ERROR)
    robot_position = move_forward(connection, SERVER_MOVE, client_ok_lengh, SERVER_SYNTAX_ERROR)
    robot_position = turn_right(connection, SERVER_TURN_RIGHT, client_ok_lengh, SERVER_SYNTAX_ERROR)
    robot_position = move_forward(connection, SERVER_MOVE, client_ok_lengh, SERVER_SYNTAX_ERROR)
    robot_position = move_forward(connection, SERVER_MOVE, client_ok_lengh, SERVER_SYNTAX_ERROR)
    robot_position = turn_right(connection, SERVER_TURN_RIGHT, client_ok_lengh, SERVER_SYNTAX_ERROR)
    robot_position = move_forward(connection, SERVER_MOVE, client_ok_lengh, SERVER_SYNTAX_ERROR)
    robot_position = turn_left(connection, SERVER_TURN_LEFT, client_ok_lengh, SERVER_SYNTAX_ERROR)
    return robot_position

def check_obstacle(newX,newY,prevX,prevY, robot_position,crashed,max_crashes,connection, SERVER_MOVE, SERVER_TURN_RIGHT,

                                                            SERVER_TURN_LEFT, client_ok_lengh,SERVER_SYNTAX_ERROR,SERVER_LOGOUT):
    if newX == prevX and newY == prevY:
        robot_position = get_around_obstacle(connection, SERVER_MOVE, SERVER_TURN_RIGHT,
                                             SERVER_TURN_LEFT, client_ok_lengh, SERVER_SYNTAX_ERROR)
        prevX, prevY = newX, newY
        newX, newY = get_coordinates(robot_position, SERVER_SYNTAX_ERROR, connection)
        crashed += 1
        print(f'crash: {crashed}')
        #if crashed == max_crashes:
           # connection.sendall(SERVER_LOGOUT)
    return  prevX, prevY, newX, newY, crashed, robot_position


def main():
    SERVER_CONFIRMATION = 0  # Zpráva s potvrzovacím kódem. Může obsahovat maximálně 5 čísel a ukončovací sekvenci \a\b.
    SERVER_MOVE = '102 MOVE\a\b'.encode('ascii')  # Příkaz pro pohyb o jedno pole vpřed
    SERVER_TURN_LEFT = '103 TURN LEFT\a\b'.encode('ascii')
    SERVER_TURN_RIGHT = '104 TURN RIGHT\a\b'.encode('ascii')
    SERVER_PICK_UP = '105 GET MESSAGE\a\b'.encode('ascii')  # Příkaz pro vyzvednutí zprávy
    SERVER_LOGOUT = '106 LOGOUT\a\b'.encode('ascii')  # Příkaz pro ukončení spojení po úspěšném vyzvednutí zprávy
    SERVER_KEY_REQUEST = '107 KEY REQUEST\a\b'.encode('ascii')  # Žádost serveru o Key ID pro komunikaci
    SERVER_OK = '200 OK\a\b'.encode('ascii')  # Kladné potvrzení
    SERVER_LOGIN_FAILED = '300 LOGIN FAILED\a\b'.encode('ascii')  # Nezdařená autentizace
    SERVER_SYNTAX_ERROR = '301 SYNTAX ERROR\a\b'.encode('ascii')  # Chybná syntaxe zprávy
    SERVER_LOGIC_ERROR = '302 LOGIC ERROR\a\b'.encode('ascii')  # Zpráva odeslaná ve špatné situaci
    SERVER_KEY_OUT_OF_RANGE_ERROR = '303 KEY OUT OF RANGE\a\b'.encode('ascii')  # Key ID není v očekávaném rozsahu
    CLIENT_USERNAME = ""  # Zpráva s uživatelským jménem. Jméno může být libovolná sekvence znaků kromě kromě dvojice \a\b.
    client_un_lengh = 20
    CLIENT_KEY_ID = ""  # Zpráva obsahující Key ID. Může obsahovat pouze celé číslo o maximálně třech cifrách.
    client_kid_lengh = 5
    CLIENT_CONFIRMATION = ""  # Zpráva s potvrzovacím kódem. Může obsahovat maximálně 5 čísel a ukončovací sekvenci \a\b.
    client_conf_lengh = 7
    CLIENT_OK = ""  # Potvrzení o provedení pohybu, kde x a y jsou souřadnice robota po provedení pohybového příkazu.
    client_ok_lengh = 12
    CLIENT_RECHARGING = 'RECHARGING\a\b'.encode(
        'ascii')  # Robot se začal dobíjet a přestal reagovat na zprávy.   #max_lengh=12
    CLIENT_FULL_POWER = 'FULL POWER\a\b'.encode('ascii')  # Robot doplnil energii a opět příjímá příkazy.
    client_fp_lengh = 12
    CLIENT_MESSAGE = ""  # Text vyzvednutého tajného vzkazu. Může obsahovat jakékoliv znaky kromě ukončovací sekvence \a\b.
    client_mess_lengh = 100
    TIMEOUT = 1  # Server i klient očekávají od protistrany odpověď po dobu tohoto intervalu.
    TIMEOUT_RECHARGING = 5  # Časový interval, během kterého musí robot dokončit dobíjení.

    sock = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
    # Bind the socket to the port
    server_address = ('192.168.56.1', 3999)
    print(f'starting up on {server_address[0]} port {server_address[1]}')
    sock.bind(server_address)
    # Listen for incoming connections
    sock.listen(1)

    while True:
        # Wait for a connection
        print('waiting for a connection')
        connection, client_address = sock.accept()
        connection.settimeout(TIMEOUT)
        try:
            while True:

                    print('connection from', client_address)
                    # Receive the data in small chunks and retransmit it
                    CLIENT_USERNAME = recieving_from_chunks(connection, client_un_lengh, SERVER_SYNTAX_ERROR)
                    print(f'received {CLIENT_USERNAME}')
                    ascii_suma = 0
                    id_hash = 0
                    if CLIENT_USERNAME == '0':
                        print('CLOSE226')
                        break

                    else:
                        for i in CLIENT_USERNAME:
                            ascii_suma += ord(i)

                        id_hash = (ascii_suma * 1000) % 65536
                        connection.sendall(SERVER_KEY_REQUEST)
                        print(ascii_suma, id_hash)
                        key_id = recieving_from_chunks(connection, client_kid_lengh, SERVER_SYNTAX_ERROR)

                        if not key_id.isnumeric():
                            connection.sendall(SERVER_SYNTAX_ERROR)
                            print('CLOSE241')
                            break
                        CLIENT_KEY_ID = int(key_id)
                        print('klic', CLIENT_KEY_ID)
                        if CLIENT_KEY_ID > 4 or CLIENT_KEY_ID < 0:
                            connection.sendall(SERVER_KEY_OUT_OF_RANGE_ERROR)
                            print('CLOSE247')
                            break

                        #1. v podlistu je server key a 2. je client key
                        else :
                            keys=[[23019,32037],[32037,29295],[18789,13603],[16443,29533],[18189,21952]]

                        SERVER_CONFIRMATION = calc_hash(id_hash, keys[CLIENT_KEY_ID][0])
                        SERVER_CONFIRMATION = data_send(SERVER_CONFIRMATION)
                        print(SERVER_CONFIRMATION)
                        connection.sendall(SERVER_CONFIRMATION)

                        CLIENT_CONFIRMATION = recieving_from_chunks(connection, client_conf_lengh, SERVER_SYNTAX_ERROR)
                        if CLIENT_CONFIRMATION == '0':
                            print('Close 265')
                            break
                        print(CLIENT_CONFIRMATION)
                        hashkey_client_final = calc_hash(id_hash, keys[CLIENT_KEY_ID][1])
                        hashkey_client_final = str(hashkey_client_final)
                        print(hashkey_client_final)

                        if (not CLIENT_CONFIRMATION.isnumeric()):
                            connection.sendall(SERVER_SYNTAX_ERROR)
                            print('Close 279')
                            break

                        elif hashkey_client_final == CLIENT_CONFIRMATION:
                            connection.sendall(SERVER_OK)
                            connection.sendall(SERVER_TURN_LEFT)
                            robot_position = recieving_from_chunks(connection, client_ok_lengh, SERVER_SYNTAX_ERROR)
                            if robot_position=='0':#nevyskytuje se
                                print('close 288')
                                break
                            print(robot_position)
                            prevX,prevY = get_coordinates(robot_position, SERVER_SYNTAX_ERROR, connection)
                            if prevX=='0' and prevY == '0':
                                print('close 292')
                                break
                            connection.sendall(SERVER_MOVE)
                            robot_position = recieving_from_chunks(connection, client_ok_lengh, SERVER_SYNTAX_ERROR)
                            print(robot_position)

                            direction = ""
                            newX, newY = get_coordinates(robot_position, SERVER_SYNTAX_ERROR, connection)
                            if newX=='0' and newY == '0':#nevyskytuje se
                                print('close 302')
                                break
                            crashed = 0
                            max_crashes = 3

                            if check_message(newX, newY, SERVER_PICK_UP, connection, CLIENT_MESSAGE, SERVER_LOGOUT,
                                          client_mess_lengh, SERVER_SYNTAX_ERROR):
                                print('CLOSE289')#nevyskytuje se
                                break

                            if newX == prevX and newY == prevY:
                               #robot_position = turn_around180(connection, SERVER_TURN_RIGHT, client_ok_lengh, SERVER_SYNTAX_ERROR)
                               robot_position = get_around_obstacle(connection, SERVER_MOVE, SERVER_TURN_RIGHT,
                                                                     SERVER_TURN_LEFT, client_ok_lengh, SERVER_SYNTAX_ERROR)
                               prevX, prevY = newX,newY
                               newX, newY = get_coordinates(robot_position, SERVER_SYNTAX_ERROR, connection)
                               crashed+=1
                               print(crashed)

                            if prevY == newY:
                                if prevX > newX:
                                    direction = 'moving left'
                                elif prevX < newX:
                                    direction = 'moving right'
                                print(direction)
                                if direction == 'moving left' and newX < 0:
                                    robot_position = turn_around180(connection, SERVER_TURN_RIGHT, client_ok_lengh,
                                                                    SERVER_SYNTAX_ERROR)
                                    direction = 'moving right'
                                elif direction == 'moving right' and newX > 0:
                                    robot_position = turn_around180(connection, SERVER_TURN_RIGHT, client_ok_lengh,
                                                                    SERVER_SYNTAX_ERROR)
                                    direction = 'moving left'
                                else:
                                    pass

                                while newX != 0:
                                    print(newX, direction, newY, prevX)
                                    robot_position = move_forward(connection, SERVER_MOVE, client_ok_lengh,
                                                                  SERVER_SYNTAX_ERROR)
                                    prevX, prevY = newX, newY
                                    newX, newY = get_coordinates(robot_position, SERVER_SYNTAX_ERROR, connection)
                                    if ((newX==-1 and direction=='moving right' and newY<0) or (newX==1 and direction=='moving left' and newY>0)) and newX==prevX:
                                        robot_position,newX,newY,prevX,prevY= DirectionLRLtoZero(connection,
                                                                                                 SERVER_TURN_LEFT,
                                                                                                 client_ok_lengh,
                                                                                                 SERVER_SYNTAX_ERROR,
                                                                                                 robot_position,
                                                                                                 SERVER_MOVE,
                                                                                                 SERVER_TURN_RIGHT, newX,
                                                                                                 newY)
                                        direction=''
                                    elif ((newX==-1 and direction=='moving right' and newY>0) or (newX==1 and direction=='moving left' and newY<0) ) and newX==prevX:
                                        robot_position,newX,newY,prevX,prevY= DirectionRLRtoZero(connection,
                                                                                                       SERVER_TURN_LEFT,
                                                                                                       client_ok_lengh,
                                                                                                       SERVER_SYNTAX_ERROR,
                                                                                                       robot_position,
                                                                                                       SERVER_MOVE,
                                                                                                       SERVER_TURN_RIGHT,
                                                                                                       newX, newY)
                                        direction=''
                                    else:
                                        prevX, prevY, newX, newY, crashed, robot_position = check_obstacle(newX,
                                                                                                                   newY,
                                                                                                                   prevX,
                                                                                                                   prevY,
                                                                                                                   robot_position,
                                                                                                                   crashed,
                                                                                                                   max_crashes,
                                                                                                                   connection,
                                                                                                                   SERVER_MOVE,
                                                                                                                   SERVER_TURN_RIGHT,
                                                                                                                   SERVER_TURN_LEFT,
                                                                                                                   client_ok_lengh,
                                                                                                                   SERVER_SYNTAX_ERROR,
                                                                                                                   SERVER_LOGOUT)

                                if (newY > 0 and direction == 'moving right') or (newY < 0 and direction == 'moving left'):
                                    robot_position = turn_right(connection, SERVER_TURN_RIGHT, client_ok_lengh, SERVER_SYNTAX_ERROR)
                                elif (newY < 0 and direction == 'moving right') or (newY > 0 and direction == 'moving left'):
                                    robot_position = turn_left(connection, SERVER_TURN_LEFT, client_ok_lengh, SERVER_SYNTAX_ERROR)
                                while newY != 0:
                                    robot_position = move_forward(connection, SERVER_MOVE, client_ok_lengh, SERVER_SYNTAX_ERROR)
                                    print(robot_position)
                                    prevX, prevY = newX, newY
                                    newX, newY = get_coordinates(robot_position, SERVER_SYNTAX_ERROR, connection)
                                    prevX, prevY, newX, newY, crashed, robot_position= check_obstacle(newX, newY,
                                                                                                               prevX, prevY,
                                                                                                               robot_position,
                                                                                                               crashed,
                                                                                                               max_crashes,
                                                                                                               connection,
                                                                                                               SERVER_MOVE,
                                                                                                               SERVER_TURN_RIGHT,
                                                                                                               SERVER_TURN_LEFT,
                                                                                                               client_ok_lengh,
                                                                                                               SERVER_SYNTAX_ERROR,
                                                                                                               SERVER_LOGOUT)
                            elif prevX == newX:
                                if prevY > newY:
                                    direction = 'moving down'
                                if prevY < newY:
                                    direction = 'moving up'
                                print(direction)
                                if direction == 'moving down' and newY < 0:
                                    robot_position = turn_around180(connection, SERVER_TURN_RIGHT, client_ok_lengh,
                                                                    SERVER_SYNTAX_ERROR)
                                    direction = 'moving up'
                                elif direction == 'moving up' and newY > 0:
                                    robot_position = turn_around180(connection, SERVER_TURN_RIGHT, client_ok_lengh,
                                                                    SERVER_SYNTAX_ERROR)
                                    direction = 'moving down'
                                else:
                                    pass

                                while newY != 0:
                                    time.sleep(0.5)
                                    print(newY, direction, newX, prevY)
                                    robot_position = move_forward(connection, SERVER_MOVE, client_ok_lengh,
                                                                  SERVER_SYNTAX_ERROR)
                                    prevX, prevY = newX, newY
                                    newX, newY =  get_coordinates(robot_position, SERVER_SYNTAX_ERROR, connection)

                                    if ((newY==-1 and direction=='moving up' and newX>0) or (newY==1 and direction=='moving down' and newX<0)) and newY==prevY:
                                        robot_position,newX,newY,prevX,prevY= DirectionLRLtoZero(connection,
                                                                                                       SERVER_TURN_LEFT,
                                                                                                       client_ok_lengh,
                                                                                                       SERVER_SYNTAX_ERROR,
                                                                                                       robot_position, SERVER_MOVE,
                                                                                                       SERVER_TURN_RIGHT, newX, newY)
                                        direction=''
                                    elif ((newY==-1 and direction=='moving up' and newX<0) or (newY==1 and direction=='moving down' and newX>0) ) and newY==prevY:
                                        robot_position,newX,newY,prevX,prevY= DirectionRLRtoZero(connection,
                                                                                                       SERVER_TURN_LEFT,
                                                                                                       client_ok_lengh,
                                                                                                       SERVER_SYNTAX_ERROR,
                                                                                                       robot_position,
                                                                                                       SERVER_MOVE,
                                                                                                       SERVER_TURN_RIGHT,
                                                                                                       newX, newY)
                                        direction=''
                                    else:

                                        prevX, prevY, newX, newY, crashed, robot_position = check_obstacle(newX,
                                                                                                                   newY,
                                                                                                                   prevX,
                                                                                                                   prevY,
                                                                                                                   robot_position,
                                                                                                                   crashed,
                                                                                                                   max_crashes,
                                                                                                                   connection,
                                                                                                                   SERVER_MOVE,
                                                                                                                   SERVER_TURN_RIGHT,
                                                                                                                   SERVER_TURN_LEFT,
                                                                                                                   client_ok_lengh,
                                                                                                                   SERVER_SYNTAX_ERROR,
                                                                                                                   SERVER_LOGOUT)

                                if (newX > 0 and direction == 'moving down') or (newX < 0 and direction == 'moving up'):
                                    robot_position = turn_right(connection, SERVER_TURN_RIGHT, client_ok_lengh, SERVER_SYNTAX_ERROR)
                                elif (newX < 0 and direction == 'moving down') or (newX > 0 and direction == 'moving up'):
                                    robot_position = turn_left(connection, SERVER_TURN_LEFT, client_ok_lengh, SERVER_SYNTAX_ERROR)
                                while newX != 0:
                                    robot_position = move_forward(connection, SERVER_MOVE, client_ok_lengh, SERVER_SYNTAX_ERROR)
                                    print(robot_position)
                                    prevX, prevY = newX, newY
                                    newX, newY =get_coordinates(robot_position, SERVER_SYNTAX_ERROR, connection)
                                    prevX, prevY, newX, newY, crashed, robot_position = check_obstacle(newX, newY,
                                                                                                               prevX, prevY,
                                                                                                               robot_position,
                                                                                                               crashed,
                                                                                                               max_crashes,
                                                                                                               connection,
                                                                                                               SERVER_MOVE,
                                                                                                               SERVER_TURN_RIGHT,
                                                                                                               SERVER_TURN_LEFT,
                                                                                                               client_ok_lengh,
                                                                                                               SERVER_SYNTAX_ERROR,
                                                                                                               SERVER_LOGOUT)

                            if check_message(newX,newY, SERVER_PICK_UP, connection, CLIENT_MESSAGE, SERVER_LOGOUT,
                                          client_mess_lengh, SERVER_SYNTAX_ERROR):
                                print('CLOSE465')
                                break


                        elif hashkey_client_final != CLIENT_CONFIRMATION:
                            connection.sendall(SERVER_LOGIN_FAILED)
                            print('CLOSE471')
                            break

        except socket.timeout:
            print('Timeouted')

        finally:
            print('CLOSE')
            #Clean up the connection
            connection.close()


if __name__ == '__main__':
    main()
