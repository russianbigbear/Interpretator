## Часть интерпретатора по предмету "Теория языков программирования"
### Задание:
Программа: главная программа языка С++. Допускается описание функций с параметрами, допустимых в программе типов.<br/>
Функции возвращают значения.<br/><br/>
**Типы данных:** double, int.<br/>
**Операции:** арифметические, сдвига и сравнения.<br/>
**Операторы:** присваивания, while.<br/>
**Операнды:** простые переменные, константы.<br/>
**Константы:** целые, символьные.<br/>

### Пример кода из файла Input.txt, работающего в написанном интерпретаторе:
	double x = 3;
	int y = 0xC;

	int func(double a) {
		int b = a * 5;
		int l = b - 3;

		return b * l;
	}

	double func2(int a){
		int b = a * a;
		int copy_b = b;	

		int counter = 0;
		while(copy_b + counter != 150)
			counter++;	

		return counter;
	}

	int main() {
		double z = func(x);
		double n = func2(y);		
		int k = ((z * 3) - 1) * 2; 

		return;
	}
