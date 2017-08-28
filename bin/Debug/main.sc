type Object(){
	var = "variable";
	inte = 10;
	type Plus(a, b){
		size = params.size();
		cout("Plus params: " + params.size());
		cout("This is a: " + params.a);
		s = 2;

		return params.a + params.b;
	}
}

va = Object();
ggg = Object().Plus();
cout("GGG = " + ggg);
//sum = va.inte;
//sum = va.Plus(2, 4);
//Object(1, 2, 4, 5);
sum = va.Plus(2, 3);
cout("Summa = " + sum);
cout(Object.Plus(6, 3));