<?php
//Принимаем почтовые данные
$name=$_POST['name'];
$phone=$_POST['phone'];
$what=$_POST['what'];
$question=$_POST['question'];
//Тут указываем на какой ящик послать письмо
$to = "nevol9shka1@mail.ru"
//Далее идет тема и само сообщение
$subject = "Заявка с сайта Brainstrom";
$message ="
Письмо отправлено из моей формы.<br />
Пользователь хочет: ".htmlspecialchars($what)."<br />
Имя: ".htmlspecialchars($name)."<br />
Телефон: ".htmlspecialchars($phone);
$headers = "From: mysiste.ru <siste-email@mysiste.ru>\r\nContent-type: text/html;
charset=utf-8 \r\n";
mail ($to, $subject, $message, $hedaers);
header('Location: thanks.html');
exit();
 ?>
