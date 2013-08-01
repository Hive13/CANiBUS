/*
 * Generic OBD package.
 *
 * WMI codes are from: http://en.wikibooks.org/wiki/Vehicle_Identification_Numbers_(VIN_codes)/World_Manufacturer_Identifier_(WMI)
 *
 * Craig Smith <craig@hive13.org>
 */

package obd

import (
)

var WMICodes = map[string]string {
	"AFA" : "Ford South Africa",
	"AAV" : "Volkswagen South Africa",
	"JA3" : "Mitsubishi",
	"JA"  : "Isuzu",
	"JF"  : "Subaru",
	"JHM" : "Honda",
	"JHG" : "Honda",
	"JHL" : "Honda",
	"JK"  : "Kawasaki",
	"JM"  : "Mazda",
	"JN"  : "Nissan",
	"JS"  : "Suzuki",
	"JT"  : "Toyota",
	"KL"  : "Daewoo GM South Korea",
	"KM8" : "Hyndai",
	"KMH" : "Hyndai",
	"KNA" : "Kia",
	"KNB" : "Kia",
	"KNC" : "Kia",
	"KNM" : "Renault Samsung",
	"KPA" : "Ssangyong",
	"KPT" : "Ssangyong",
	"L56" : "Renault Samsung",
	"L5Y" : "Merato Motocycle",
	"LDY" : "Zhongtong Coach China",
	"LGH" : "Dong Feng China",
	"LKL" : "Suzhou King Long China",
	"LSY" : "Brillance Zhonghua",
	"LTV" : "Toyota Tian Jin",
	"LVS" : "Ford Chang An",
	"LVV" : "Chery China",
	"LZM" : "MAN China",
	"LZE" : "Isuzu Guangzhou China",
	"LZG" : "Shaanxi China",
	"LZY" : "Yutong Zhengzhou China",
	"MA1" : "Mahindra",
	"MA3" : "Suzuki India",
	"MA7" : "Honda Siel India",
	"MAL" : "Hyundai",
	"MHR" : "Honda Indonesia",
	"MNB" : "Ford Thailand",
	"MNT" : "Nissan Thailand",
	"MM8" : "Mazda Thailand",
	"MPA" : "Isuzu Thailand",
	"MP1" : "Isuzu Thailand",
	"MRH" : "Honda Thailand",
	"MR0" : "Toyoto Thailand",
	"NLE" : "Mercedes-Benz Turk Truck",
	"NM0" : "Ford Turkey",
	"NM4" : "Tofas Turk",
	"NMT" : "Toyota Turkiye",
	"PE1" : "Ford Phillipines",
	"PE3" : "Mazda Phillipines",
	"PL1" : "Proton Malaysia",
	"SAL" : "Land Rover",
	"SAJ" : "Jaguar",
	"SAR" : "Rover",
	"SCA" : "Rolls Royce",
	"SCC" : "Lotus Cars",
	"SCE" : "DeLorean North Ireland",
	"SCF" : "Aston",
	"SDB" : "Peugeot UK",
	"SFD" : "Alexander Dennis UK",
	"SHS" : "Honda UK",
	"SJN" : "Nissan UK",
	"SU9" : "Solaris Bus & Coach Poland",
	"TK9" : "SOR Czech Republic",
	"TDM" : "QUANTYA Switzerland",
	"TMB" : "Škoda Czech Republic",
	"TMK" : "Karosa Czech Republic",
	"TMP" : "Škoda trolleybuses Czech Republic",
	"TMT" : "Tatra Czech Republic",
	"TM9" : "Škoda trolleybuses Czech Republic",
	"TN9" : "Karosa Czech Republic",
	"TRA" : "Ikarus Bus",
	"TSE" : "Ikarus Egyedi Hungary",
	"TSM" : "Suzuki Hungary",
	"UU1" : "Renault Dacia Romania",
	"VF1" : "Renault",
	"VF3" : "Peugeot",
	"VF6" : "Renault Truck/Bus",
	"VF7" : "Citroën",
	"VF8" : "Matra",
	"VLU" : "Scania France",
	"VNE" : "Irisbus France",
	"VSE" : "Suzuki Spain",
	"VSK" : "Nissan Spain",
	"VSS" : "SEAT",
	"VSX" : "Open Spain",
	"VS6" : "Ford Spain",
	"VS9" : "Carrocerias Ayats Spain",
	"VWV" : "Volkswagen Spain",
	"VX1" : "Zastava / Yugo Serbia",
	"WAG" : "Neoplan",
	"WAU" : "Audi",
	"WBA" : "BMW",
	"WBS" : "BMW M",
	"WDB" : "Mercedes-Benz",
	"WDC" : "DaimlerChrysler",
	"WDD" : "McLaren",
	"WEB" : "Evobus GmbH (Mercedes-Bus)",
	"WF0" : "Ford Germany",
	"WMA" : "MAN Germany",
	"WMW" : "MINI",
	"WP0" : "Porche",
	"W0L" : "Opel",
	"WVW" : "Volkswagen",
	"WV1" : "Volkswagen Commercial",
	"WV2" : "Volkswagen Bus/Van",
	"XL9" : "Spyker",
	"XMC" : "Mitsubishi (NedCar)",
	"XTA" : "Lada/AutoVaz Russia",
	"YK1" : "Saab",
	"YS2" : "Scania AB",
	"YS3" : "Saab",
	"YS4" : "Scania AB",
	"YV1" : "Volvo",
	"YV4" : "Volvo",
	"YV2" : "Volvo Trucks",
	"YV3" : "Volvo Buses",
	"ZAM" : "Maserati Biturbo",
	"ZAP" : "Piaggio/Vespa/Gilera",
	"ZAR" : "Alfa Romeo",
	"ZCG" : "Cagiva SpA",
	"ZDM" : "Ducati SpA",
	"ZDF" : "Ferrari Dino",
	"ZD4" : "Aprilia",
	"ZFA" : "Fiat",
	"ZFC" : "Fiat V.I.",
	"ZFF" : "Ferrari",
	"ZHW" : "Lamborghini",
	"ZLA" : "Lancia",
	"ZOM" : "OM",
	"1C3" : "Chrysler",
	"1C6" : "Chrysler",
	"1D3" : "Dodge",
	"1FA" : "Ford",
	"1FB" : "Ford",
	"1FC" : "Ford",
	"1FD" : "Ford",
	"1FM" : "Ford",
	"1FT" : "Ford",
	"1FU" : "Freightliner",
	"1FV" : "Freightliner",
	"1F9" : "FWD Corp",
	"1G"  : "GM",
	"1GC" : "Chevrolet Truck",
	"1GT" : "GMC Truck",
	"1G1" : "Chevrolet",
	"1G2" : "Pontiac",
	"1G3" : "Oldsmobile",
	"1G4" : "Buick",
	"1G6" : "Cadillac",
	"1GM" : "Pontiac",
	"1G8" : "Saturn",
	"1H"  : "Honda",
	"1HD" : "Harley-Davidson",
	"1J4" : "Jeep",
	"1L"  : "Lincoln",
	"1ME" : "Mercury",
	"1M1" : "Mack Truck",
	"1M2" : "Mack Truck",
	"1M3" : "Mack Truck",
	"1M4" : "Mack Truck",
	"1N"  : "Nissan",
	"1NX" : "NUMMI",
	"1P3" : "Plymouth",
	"1R9" : "Roadrunner Hay Squeeze",
	"1VW" : "Volkswagen",
	"1XK" : "Kenworth",
	"1XP" : "Peterbilt",
	"1YV" : "Mazda",
	"2C3" : "Chrysler Canada",
	"2CN" : "CAMI",
	"2D3" : "Dodge Canada",
	"2FA" : "Ford Canada",
	"2FB" : "Ford Canada",
	"2FC" : "Ford Canada",
	"2FM" : "Ford Canada",
	"2FT" : "Ford Canada",
	"2FU" : "Freightliner",
	"2FV" : "Freightliner",
	"2FZ" : "Sterling",
	"2G"  : "GM Canada",
	"2G1" : "Chevrolet Canada",
	"2G2" : "Pontiac Canada",
	"2G3" : "Oldsmobile Canada",
	"2G4" : "Buick Canada",
	"2HG" : "Honda Canada",
	"2HK" : "Honda Canada",
	"2HM" : "Hyundai Canada",
	"2M"  : "Mercury",
	"2P3" : "Plymouth Canada",
	"2T"  : "Toyota Canada",
	"2V4" : "Volkswagen Canada",
	"2WK" : "Western Star",
	"2WL" : "Western Star",
	"2WM" : "Western Star",
	"3D3" : "Dodge Mexico",
	"3FA" : "Ford Mexico",
	"3FE" : "Ford Mexico",
	"3G"  : "GM Mexico",
	"3H"  : "Honda Mexico",
	"3N"  : "Nissan Mexico",
	"3P3" : "Plymouth Mexico",
	"3VW" : "Volkswagen Mexico",
	"4F"  : "Mazda",
	"4M"  : "Mercury",
	"4S"  : "Subaru-Isuzu",
	"4T"  : "Toyota",
	"4US" : "BMW",
	"4UZ" : "Frt-Thomas Bus",
	"4V1" : "Volvo",
	"4V2" : "Volvo",
	"4V3" : "Volvo",
	"4V4" : "Volvo",
	"4V5" : "Volvo",
	"4V6" : "Volvo",
	"4VL" : "Volvo",
	"4VM" : "Volvo",
	"4VZ" : "Volvo",
	"5F"  : "Honda USA-Alabama",
	"5L"  : "Lincoln",
	"5N1" : "Nissan",
	"5NP" : "Nyundai",
	"5T"  : "Toyota Truck",
	"5YJ" : "Tesla",
	"6AB" : "MAN Australia",
	"6F4" : "Nissan Australia",
	"6F5" : "Kenworth Australia",
	"6FP" : "Ford Australia",
	"6G1" : "GM Holden",
	"6G2" : "Pontiac Australia",
	"6H8" : "GM Holden",
	"6MM" : "Mitsubishi Australia",
	"6T1" : "Toyota Australia",
	"6U9" : "Privately Imported Australia",
	"8AG" : "Chevrolet Argentina",
	"8GG" : "Chevrolet Chile",
	"8AP" : "Fiat Argentina",
	"8AF" : "Ford Argentina",
	"8AD" : "Peugeot Argentina",
	"8GD" : "Peugeot Chile",
	"8A1" : "Renault Argentina",
	"8AK" : "Suzuki Argentina",
	"8AJ" : "Toyota Argentina",
	"8AW" : "Volkswagen Argentina",
	"93U" : "Audi Brazil",
	"9BG" : "Chevrolet Brazil",
	"935" : "Citroën Brazil",
	"9BD" : "Fiat Brazil",
	"9BF" : "Ford Brazil",
	"93H" : "Honda Brazil",
	"9BM" : "Mercedes-Benz Brazil",
	"936" : "Peugeot Brazil",
	"93Y" : "Renault Brazil",
	"9BS" : "Scania Brazil",
	"93R" : "Toyota Brazil",
	"9BW" : "Volkswagen Brazil",
	"9FB" : "Renault Colombia",
}

var GMSeries = map[string]string {
	"CW" : "Park Avenue",
	"CD" : "DeVille",
	"CT" : "DeVille",
	"CB" : "Fleetwood",
	"CG" : "Sixty Special",
	"DW" : "Brougham",
	"DV" : "Terraza",
	"DG" : "CTS",
	"DM" : "CTS",
	"DP" : "CTS",
	"DR" : "CTS",
	"DU" : "CTS",
	"EC" : "G8",
	"EL" : "Eldorado",
	"ET" : "Eldorado",
	"EZ" : "Riviera",
	"AP" : "Camaro",
	"AS" : "Firebird",
	"AV" : "Firebird Formula/ Trans Am",
	"AW" : "Firebird Formula/ Trans Am",
	"FP" : "Camaro",
	"FS" : "Firebird",
	"FV" : "Firebird Formula/ Trans Am",
	"FW" : "Firebird Formula/ Trans Am",
	"GR" : "Riviera or Aurora",
	"GS" : "Riviera or Aurora",
	"HP" : "Le Sabre Custom",
	"HR" : "Le Sabre Limited",
	"HX" : "Bonneville SE",
	"HY" : "Bonneville SLE",
	"HZ" : "Bonnevlle SSEi",
	"JB" : "Sunfire",
	"JC" : "Cavalier",
	"JF" : "Cavalier",
	"JH" : "Cavalier Z24",
	"JT" : "Saturn L200",
	"JU" : "Saturn L200",
	"JW" : "Saturn L300",
	"KD" : "Deville",
	"KE" : "Deville",
	"KF" : "Deville",
	"KS" : "Seville / SLS",
	"KY" : "Seville / STS",
	"LD" : "Corsica Base",
	"LT" : "Corsica LT",
	"LV" : "Beretta GT",
	"LW" : "Beretta GT RPO Z21 & Z26",
	"LZ" : "Corisca LTZ RPO Z54 / Beratta GTZ RPO Z04",
	"ND" : "Malibu",
	"NE" : "Malibu LS | Grand Am SE",
	"NF" : "Grand Am SE1 | Alero GLS",
	"NG" : "Grand Am SE2",
	"NK" : "Alero Level I",
	"NL" : "Alero Level II",
	"NV" : "Grand Am GT1",
	"NW" : "Grand Am GT",
	"PE" : "Fiero Coupe",
	"PF" : "Fiero SE",
	"PG" : "Fiero GT",
	"PM" : "Fier Sport Coupe",
	"SL" : "Vibe",
	"SM" : "Vibe AWD",
	"SN" : "Vibe GT",
	"VR" : "Allanté | Catera",
	"VS" : "Allanté | Catera",
	"VX" : "GTO Coupe",
	"WB" : "Impala | Regal LS",
	"WC" : "Lacrosse | Allure CX",
	"WD" : "Lacrosse | Allure CXL",
	"WE" : "Lacrosse | Allure CXS",
	"WF" : "Impala | Regal GS",
	"WH" : "Impala LS",
	"WJ" : "Grand Prix SE",
	"WK" : "Grand Prix SE1",
	"WL" : "Lumina",
	"WP" : "Grand Prix GT | Impala",
	"WR" : "Grand Prix GTP",
	"WW" : "Monte Carlo LS",
	"WX" : "Monte Carlo SS",
	"WY" : "Century Limited",
	"YV" : "XLR",
	"YY" : "Corvette",
	"YZ" : "Corvette",
	"ZT" : "LS",
	"ZS" : "Base",
	"ZU" : "LT",
	"ZW" : "SS",
	"ZG" : "Base",
	"ZH" : "GT",
	"ZM" : "GTP",
	"ZF" : "Base 4 cyl",
}

type VehicleAttributes struct {
	Model string
	BodyType string
	RestraintSystem string
	ChargerType string
	Motor string
	ProductionSeries string
}

func GetMakeFromVIN(vin string) string {
	wmi := []byte(vin)
	wmi = wmi[:3]
	model, ok := WMICodes[string(wmi)]
	if !ok {  // Try a 2 digit code instead
		model, ok = WMICodes[string(wmi[:2])]
		if !ok {
			model = "Unknown"
		}
	}
	return model
}


// This method varies per manufacturer
func GetModelFromVIN(vin string) VehicleAttributes {
	vds := []byte(vin)
	wmi := string(vds[0:3])
	vds = vds[3:8]
	manuf := GetMakeFromVIN(vin)
	v := VehicleAttributes{Model: "Unkown"}
	if manuf == "Tesla" {
		v = parseTeslaVDS(vds, vin, v)
	} else if wmi == "1G1" || wmi == "2G1" || wmi == "3G1" || wmi == "KL1" || wmi == "9BG" || wmi == "8AG" || wmi == "8GG" || wmi == "8Z1" || wmi == "KLA" || wmi == "1G2" || wmi == "2G2" || wmi == "3G2" || wmi == "3G2" || wmi == "5Y2" || wmi == "6G2" || wmi == "KL2" || wmi == "KL2" || wmi == "1G3" || wmi == "2G3" || wmi == "1G4" || wmi == "2G4" || wmi == "3G4" || wmi == "1G6" || wmi == "W06" || wmi == "1G8" || wmi == "W08" {
		v = parseGMVDS(vds, vin, v)
	}
	return v
}

func GetYearFromVIN(vin string) string {
	y := []byte(vin)[9]
	r := []byte(vin)[6]
	year := "Unknown"
	if r >= 48 && r <= 57 { // Numeric 1980-2009
		if y == 65 {
			year = "1980"
		} else if y == 66 {
			year = "1981"
		} else if y == 67 {
			year = "1982"
		} else if y == 68 {
			year = "1983"
		} else if y == 69 {
			year = "1984"
		} else if y == 70 {
			year = "1985"
		} else if y == 71 {
			year = "1986"
		} else if y == 72 {
			year = "1987"
		} else if y == 74 {
			year = "1988"
		} else if y == 75 {
			year = "1989"
		} else if y == 76 {
			year = "1990"
		} else if y == 77 {
			year = "1991"
		} else if y == 78 {
			year = "1992"
		} else if y == 80 {
			year = "1993"
		} else if y == 82 {
			year = "1994"
		} else if y == 83 {
			year = "1995"
		} else if y == 84 {
			year = "1996"
		} else if y == 86 {
			year = "1997"
		} else if y == 87 {
			year = "1998"
		} else if y == 88 {
			year = "1999"
		} else if y == 89 {
			year = "2000"
		} else if y == 49 {
			year = "2001"
		} else if y == 50 {
			year = "2002"
		} else if y == 51 {
			year = "2003"
		} else if y == 52 {
			year = "2004"
		} else if y == 53 {
			year = "2005"
		} else if y == 54 {
			year = "2006"
		} else if y == 55 {
			year = "2007"
		} else if y == 56 {
			year = "2008"
		} else if y == 57 {
			year = "2009"
		}
	} else {
		if y == 65 {
			year = "2010"
		} else if y == 66 {
			year = "2011"
		} else if y == 67 {
			year = "2012"
		} else if y == 68 {
			year = "2013"
		} else if y == 69 {
			year = "2014"
		} else if y == 70 {
			year = "2015"
		} else if y == 71 {
			year = "2016"
		} else if y == 72 {
			year = "2017"
		} else if y == 74 {
			year = "2018"
		} else if y == 75 {
			year = "2019"
		} else if y == 76 {
			year = "2020"
		} else if y == 77 {
			year = "2021"
		} else if y == 78 {
			year = "2022"
		} else if y == 80 {
			year = "2023"
		} else if y == 82 {
			year = "2024"
		} else if y == 83 {
			year = "2025"
		} else if y == 84 {
			year = "2026"
		} else if y == 86 {
			year = "2027"
		} else if y == 87 {
			year = "2028"
		} else if y == 88 {
			year = "2029"
		} else if y == 89 {
			year = "2030"
		} else if y == 49 {
			year = "2031"
		} else if y == 50 {
			year = "2032"
		} else if y == 51 {
			year = "2033"
		} else if y == 52 {
			year = "2034"
		} else if y == 53 {
			year = "2035"
		} else if y == 54 {
			year = "2036"
		} else if y == 55 {
			year = "2037"
		} else if y == 56 {
			year = "2038"
		} else if y == 57 {
			year = "2039"
		}
	}
	return year
}

////////////////////////
/// VIN/VDS Decoders ///
////////////////////////
func parseGMVDS(vds []byte, vin string, v VehicleAttributes) VehicleAttributes {
	series := string(vds[0:2])
	m, ok := GMSeries[series]
	if ok {
		v.Model = m
	}
	if vds[2] == 49 {
		v.BodyType = "Two-Door Coupe"
	} else if vds[2] == 50 {
		v.BodyType = "Two-Door"
	} else if vds[2] == 51 {
		v.BodyType = "Two-Door Convertible"
	} else if vds[2] == 53 {
		v.BodyType = "Four-Door Sedan"
	} else if vds[2] == 54 {
		v.BodyType = "Four-Door Sedan Hatchback"
	} else if vds[2] == 56 {
		v.BodyType = "Four-Door Sedan Wagon/Two-Door Hatchback"
	} else if vds[2] == 57 {
		v.BodyType = "Four-Door Station Wagon"
	}
	if vds[4] == 49 {
		v.Motor = "3.8L V6 SF1 Fuel"
	} else if vds[4] == 50 {
		v.Motor = "1.3L I4 MFI Fuel"
	} else if vds[4] == 52 {
		v.Motor = "2.2L I4 SFI Fuel"
	} else if vds[4] == 54 {
		v.Motor = "2.2L I4 MFI Fuel"
	} else if vds[4] == 55 {
		v.Motor = "3.6L V6 SFI Fuel"
	} else if vds[4] == 56 {
		v.Motor = "1.8L I4 MFI Fuel"
	} else if vds[4] == 57 {
		v.Motor = "4.6L V8 SFI Fuel"
	} else if vds[4] == 65 {
		v.Motor = "2.3L I4 MFI Fuel"
	} else if vds[4] == 67 {
		v.Motor = "4.0L I4 SFI Fuel"
	} else if vds[4] == 68 {
		v.Motor = "2.3L I4 MFI Fuel"
	} else if vds[4] == 69 {
		v.Motor = "3.4L V6 | 5L V8"
	} else if vds[4] == 70 {
		v.Motor = "2.2L I4 | 5L V8"
	} else if vds[4] == 71 {
		v.Motor = "5.7L V8 MFI Fuel"
	} else if vds[4] == 72 {
		v.Motor = "3.5L V6 | 5L V8"
	} else if vds[4] == 74 {
		v.Motor = "3.1L V6 SFI Fuel"
	} else if vds[4] == 75 {
		v.Motor = "3.8L V6 SFI Fuel"
	} else if vds[4] == 76 {
		v.Motor = "1.8L I4 SFI Fuel"
	} else if vds[4] == 77 {
		v.Motor = "1.4L I4 | 2.6L V6"
	} else if vds[4] == 78 {
		v.Motor = "3.2L V6 MFI Fuel"
	} else if vds[4] == 80 {
		v.Motor = "5.7L V8 | 2L SC Straight"
	} else if vds[4] == 82 {
		v.Motor = "3L V6 MFI Fuel"
	} else if vds[4] == 83 {
		v.Motor = "5.7L V8 | 2.8L V6"
	} else if vds[4] == 84 {
		v.Motor = "2.4L I4 SFI Fuel"
	} else if vds[4] == 85 {
		v.Motor = "6L V8 MFI Fuel"
	} else if vds[4] == 87 {
		v.Motor = "2.8L V6 MPI Fuel"
	} else if vds[4] == 88 {
		v.Motor = "3.1L V6 MPFI Fuel"
	} else if vds[4] == 89 {
		v.Motor = "5L V8 4BBL Fuel"
	}
	return v
}

func parseTeslaVDS(vds []byte, vin string, v VehicleAttributes) VehicleAttributes {
	v.Model = "Model " + string(vds[0])
	if vds[1] == 'A' {
		v.BodyType = "5 Door Hatchback LHD RWD"
	}
	// vds[2] = Restraint System
	if vds[3] == 65 {
		v.ChargerType = "10kw Charger"
	} else if vds[3] == 66 {
		v.ChargerType = "20kw Charger"
	} else if vds[3] == 67 {
		v.ChargerType = "10kw Charger, w/DC Fast Charge"
	} else if vds[4] == 68 {
		v.ChargerType = "20kw Charger, w/DC Fast Charge"
	}
	if vds[4] == 67 {
		v.Motor = "Base A/C Motor, Tier 2 Battery (31-40kWh)"
	} else if vds[4] == 71 {
		v.Motor = "Base A/C Motor, Tier 4 Battery (51-60 kWh)"
	} else if vds[4] == 78 {
		v.Motor = "Base A/C Motor, Tier 7 Battery (81-90 kWh)"
	} else if vds[4] == 80 {
		v.Motor = "Performance A/C Motor, Tier 7 Battery (81-90 kWh)"
	}
	// Bonus
	manuf_num :=[]byte(vin)[11]
	if manuf_num == 65 {
		v.ProductionSeries = "Alpha Prototype"
	} else if manuf_num == 66 {
		v.ProductionSeries = "Beta Prototype"
	} else if manuf_num == 82 {
		v.ProductionSeries = "Release Candidate"
	} else if manuf_num == 80 {
		v.ProductionSeries = "Production Vehicle"
	} else if manuf_num == 83 {
		v.ProductionSeries = "Signature Series"
	} else if manuf_num == 70 {
		v.ProductionSeries = "Founder Series"
	}

	return v
}
