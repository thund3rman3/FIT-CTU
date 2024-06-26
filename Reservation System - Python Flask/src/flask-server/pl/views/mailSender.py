import smtplib, ssl
from email.mime.text import MIMEText
from email.mime.multipart import MIMEMultipart


class mail_sender:
    """Implementation of sending mails.
    """

    def send_mail(self, adress, reservation_number, hall, seats):
        """Sends mail.

        Args:
            adress: customers address
            reservation_number: number of customers reservation
            hall: hall number where screening takes place
            seats: reserved seats

         Returns:
            True if email send correctly, else False
         """

        self.sender_email = "kino.vesmir@seznam.cz"
        self.receiver_email = adress
        self.password = "krab1234"

        self.message = MIMEMultipart("alternative")
        self.message["Subject"] = "Potvrzení rezervace"
        self.message["From"] = self.sender_email
        self.message["To"] = self.receiver_email

        self.text = f"""\
            Rezervace číslo {reservation_number} byla potvrzena.
            Sál {hall}       
            {seats}
        """

        self.html = f"""\
        <html>
          <body>
            <p>Dobrý den,
            <br>Rezervace číslo <b>{reservation_number}</b> byla potvrzena.<br>
            <br>Sál <b>{hall}</b><br>        
            <br>{seats}<br>
            </p>
          </body>
        </html>
        """

        part1 = MIMEText(self.text, "plain")
        part2 = MIMEText(self.html, "html")

        self.message.attach(part1)
        self.message.attach(part2)

        context = ssl.create_default_context()
        try:
            server = smtplib.SMTP_SSL("smtp.seznam.cz", 465, context=context)
            server.login(self.sender_email, self.password)
            server.sendmail(self.sender_email, self.receiver_email, self.message.as_string())
            return True
        except:
            return False
