import "./App.css";
import Button from "./components/Button/Button";
import NavBar from "./components/NavBar/NavBar";
import Main from "./components/main/main";
import Cent from "./components/cent/cent";
import Dent from "./components/cent/dent";
import Check from "./components/cent/check";
import Just from "./components/cent/justfoot";

function App() {
  return (
    <div className="App">
      <NavBar />
      <div className="landingPage">
        <div className="FoodApp">Food app</div>
        <div className="tagline">
          {`Why stay hungry when\n you can order form Bella Onojie`}
        </div>
        <div className="downloadText">
          Download the bella onoje's food app now on
        </div>
        <div className="buttons">
          <Button
            text={"Playstore"}
            borderColor={"#FA4A0C"}
            color={"#FA4A0C"}
            textColor={"white"}
          />
          <Button
            text={"App Store"}
            borderColor={""}
            color={"transparent"}
            textColor={"white"}
          />
        </div>
      </div>
      <div>
        <Main />
      </div>
      <div>
        <Cent />
      </div>
      <div>
        <Dent />
      </div>
      <div>
        <Check />
      </div>
      <div>
        <Just />
      </div>
    </div>
  );
}

export default App;
