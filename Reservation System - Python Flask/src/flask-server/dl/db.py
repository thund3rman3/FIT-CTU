from sqlalchemy import create_engine
from sqlalchemy.orm import sessionmaker


class Database:
    def __init__(self):
        """Create engine and session from database."""
        self.engine = create_engine('postgresql://ltqrygvr:hAB6Dllwe50RQWttg9gpisjc0VDj9kSp@manny.db.elephantsql.com/ltqrygvr', echo=True)

        Session = sessionmaker(self.engine)
        self.session = Session()

db = Database()
