from flask import render_template
from flask_login import LoginManager
from pl.views.searchView import searchView
from pl.views.movieView import movieView
from pl.views.adminView import adminView
from pl.views.contactView import contactView
from pl.views.reservationView import reservationView
from dl.db import db
from dl.dao import DAOMovieOperations, DAOScreeningOperations, DAOAdminOperations
import datetime
from flask import Flask
from flask_classful import FlaskView

# start Flask app and connect database
app = Flask(__name__, template_folder="pl/templates", static_folder="pl/static")

app.config[
    'SQLALCHEMY_DATABASE_URI'] = 'postgresql://ltqrygvr:hAB6Dllwe50RQWttg9gpisjc0VDj9kSp@manny.db.elephantsql.com/ltqrygvr'

app.secret_key = '_HYDwRIl6SBdo0AAEMzVbA'

dmo = DAOMovieOperations(db.session)
dso = DAOScreeningOperations(db.session)

login_manager = LoginManager()
login_manager.init_app(app)


@login_manager.user_loader
def load_user(user_id):
    """Main page of website

    Args:
        user_id: id of admin

    Returns:
        admin object from database based on id
    """
    dao = DAOAdminOperations(db.session)
    return dao.get_user_by_id(user_id)


class View(FlaskView):
    """Implementation of index page
    """
    def index(self):
        """Main page of website

        Returns:
            index.html
        """
        return render_template("index.html", film_promitani=dso.get_screening_by_time_until(datetime.date.today(),
                                                                                            datetime.date.today() + datetime.timedelta(
                                                                                                days=7)))


View.register(app)
searchView.register(app)
movieView.register(app)
reservationView.register(app)
contactView.register(app)
adminView.register(app)

if __name__ == '__main__':
    # run the application
    app.run(debug=True)
