import os

def invert_binary(string: str) -> str:
    inverted = string[::-1]
    return inverted

def pow(base: int, expo: int) -> int:
    if(expo == 0):
        return 0
    if(expo == 1):
        return base
    return base * pow(base, expo - 1)

def create_menu() -> str:
    print("Qual opção você deseja?")
    print("1. 8")
    print("2. 16")
    print("3. 32")
    print("4. 64")
    print("5. Exit")
    return str(input("> "))

def convert_bin_2_dec(string: str) -> bool:
    result = 0
    for i in range(0, len(string), 1):
        if(string[i] != '0' and string[i] != '1'):
            print("Todos os caracteres devem ser 0 ou 1")
            return False
        if(string[i] == '1'):
            result += result + pow(2, i)

    print(f"\nBinary: {invert_binary(string)}\nDecimal: {result}\n")
    return True

def main() -> None:
    done = True
    while(done):
        precision = 0
        option = create_menu()
        match option:
            case '1': precision = 8
            case '2': precision = 16
            case '3': precision = 32
            case '4': precision = 64
            case '5': 
                done = False
                os.system("clear || cls")
                break
            case _:
                print("Por favor digite apenas algo entre 1 e 5")
                continue
        
        if(precision == 0):
            print("Por favor digite um valor válido")

        binary = str(input("Digite o seu binário sem espaços\n> "))

        if(len(binary) != precision):
            print("O tamanho do binário deve ser igual a precisão escolhida")

        new_binary = invert_binary(binary)

        if not convert_bin_2_dec(new_binary):
            continue
        

if '__main__' == __name__:
    main()
