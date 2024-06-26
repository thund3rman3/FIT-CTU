# coding: utf-8
from sqlalchemy import BigInteger, Column, Date, Float, ForeignKey, Index, Integer, Numeric, String, Table, Text, Time
from sqlalchemy.orm import relationship
from sqlalchemy.dialects.postgresql import OID
from sqlalchemy.ext.declarative import declarative_base
from flask_login import UserMixin

Base = declarative_base()
metadata = Base.metadata


# definitions for entities from database model represented by classes

class Admin(Base, UserMixin):
    """Admin table definition.
    """

    __tablename__ = 'Admin'

    heslo = Column(String, nullable=False)
    email = Column(String(50), nullable=False, unique=True)
    admin_id = Column(Integer, primary_key=True)

    kinos = relationship('Kino', secondary='Spravuje')

    def get_id(self):
        return self.admin_id


class Film(Base):
    """Film table definition.
    """

    __tablename__ = 'Film'

    delka_filmu = Column(Integer, nullable=False)
    nazev = Column(String, nullable=False, unique=True)
    popis = Column(String(500), nullable=False)
    rok_vydani = Column(Numeric(4, 0))
    zanr = Column(String(50), nullable=False)
    film_id = Column(Integer, primary_key=True)


class Kino(Base):
    """Kino table definition.
    """

    __tablename__ = 'Kino'

    adresa = Column(String(50), nullable=False)
    nazev = Column(String(70), nullable=False)
    kino_id = Column(Integer, primary_key=True)


class Uzivatel(Base):
    """Uzivatel table definition.
    """
    __tablename__ = 'Uzivatel'

    email = Column(String(50), nullable=False, unique=True)
    jmeno = Column(String(50), nullable=False)
    prijmeni = Column(String(50), nullable=False)
    uzivatel_id = Column(Integer, primary_key=True)


t_pg_stat_statements = Table(
    'pg_stat_statements', metadata,
    Column('userid', OID),
    Column('dbid', OID),
    Column('queryid', BigInteger),
    Column('query', Text),
    Column('calls', BigInteger),
    Column('total_time', Float(53)),
    Column('min_time', Float(53)),
    Column('max_time', Float(53)),
    Column('mean_time', Float(53)),
    Column('stddev_time', Float(53)),
    Column('rows', BigInteger),
    Column('shared_blks_hit', BigInteger),
    Column('shared_blks_read', BigInteger),
    Column('shared_blks_dirtied', BigInteger),
    Column('shared_blks_written', BigInteger),
    Column('local_blks_hit', BigInteger),
    Column('local_blks_read', BigInteger),
    Column('local_blks_dirtied', BigInteger),
    Column('local_blks_written', BigInteger),
    Column('temp_blks_read', BigInteger),
    Column('temp_blks_written', BigInteger),
    Column('blk_read_time', Float(53)),
    Column('blk_write_time', Float(53))
)


class Sal(Base):
    """Sal table definition.
    """
    __tablename__ = 'Sal'
    __table_args__ = (
        Index('IDX_Kino_Cislo', 'cislo', 'kino_id', unique=True),
    )

    cislo = Column(Integer, nullable=False)
    sal_id = Column(Integer, primary_key=True)
    kino_id = Column(ForeignKey('Kino.kino_id'), nullable=False)

    kino = relationship('Kino')


t_Spravuje = Table(
    'Spravuje', metadata,
    Column('kino_id', ForeignKey('Kino.kino_id'), nullable=False),
    Column('admin_id', ForeignKey('Admin.admin_id'), nullable=False)
)


class Promitani(Base):
    """Promitani table definition.
    """

    __tablename__ = 'Promitani'

    cas = Column(Time, nullable=False)
    datum = Column(Date, nullable=False)
    promitani_id = Column(Integer, primary_key=True)
    film_id = Column(ForeignKey('Film.film_id'), nullable=False)
    sal_id = Column(ForeignKey('Sal.sal_id'), nullable=False)

    film = relationship('Film')
    sal = relationship('Sal')


class Sedadlo(Base):
    """Sedadlo table definition.
    """
    __tablename__ = 'Sedadlo'
    __table_args__ = (
        Index('IDX_Cislo_Rada_Sal', 'sal_id', 'rada', 'cislo', unique=True),
    )

    cislo = Column(Integer, nullable=False)
    rada = Column(Integer, nullable=False)
    sedadlo_id = Column(Integer, primary_key=True)
    sal_id = Column(ForeignKey('Sal.sal_id'), nullable=False)

    sal = relationship('Sal')


class Rezervace(Base):
    """Rezervace table definition.
    """

    __tablename__ = 'Rezervace'

    cislo_rezervace = Column(Integer, nullable=False, unique=True)
    rezervace_id = Column(Integer, primary_key=True)
    promitani_id = Column(ForeignKey('Promitani.promitani_id'), nullable=False)
    uzivatel_id = Column(ForeignKey('Uzivatel.uzivatel_id'), nullable=False)

    promitani = relationship('Promitani')
    uzivatel = relationship('Uzivatel')
    sedadlos = relationship('Sedadlo', secondary='Je_na')


t_Je_na = Table(
    'Je_na', metadata,
    Column('sedadlo_id', ForeignKey('Sedadlo.sedadlo_id'), nullable=False),
    Column('rezervace_id', ForeignKey('Rezervace.rezervace_id'), nullable=False)
)
