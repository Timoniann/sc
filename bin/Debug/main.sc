type Object(){
	var = "variabl\ae";
	inte = 10;
	type Plus(a, b){
		size = params.size();
		cout("Plus\" params: " + params.size());
		cout("This is a: " + params.a);
		s = 2;
		a = 6;
		return params.a + params.b;
	}
	type Ret(){
		//if(2 == 2);
		sw = 2;
	}

	type CoutGlobal(){
		cout("Global variable: " + intValue);
	}
	r = Ret();

}

// for (i = 0; i < 10; ++i)
// {
// 	cout("I am " + i);
// }

intValue = 234;

va = Object();
ggg = Object().Plus();
cout("GGG = " + ggg);
//sum = va.inte;
//sum = va.Plus(2, 4);
obj = Object(1, 2, 4, 5);
sum = va.Plus(2, 3);
cout("Summa = " + sum);
cout(Object.Plus(6, 3));

Object.CoutGlobal();
cout("Ticks: " + ticks());
if(ticks() == ticks())
	cout("Zero tick");
else cout("Wrong tick");

cout(ticks());
cout(ticks());
