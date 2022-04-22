// This file is the entirety of the app Component
// Vue components have data and lifecycle management functions
// Interact with the components only through lifecycle functions, never through the DOM e.g. getElementById and element.addClass("myClass")

export default {
    data() {
        // This is the state of the component. 
        // Its fields can be accessed from `this` e.g. this.arr
        // This is NOT the best way of doing this
        return { 
            arr: [
                { field: 'air_delay_on', value: 0 }, 
                { field: 'air_delay_off', value: 0 }, 
                { field: '_delay_on', value: 0 }, 
                { field: '_delay_off', value: 0 }, 
                { field: '_pulse_width', value: 0 }, 
                { field: 'source_delay_on', value: 0 }, 
                { field: 'source_delay_off', value: 0 }, 
                { field: 'source_pulse_width', value: 0 }, 
                { field: 'drain_delay_on', value: 0 }, 
                { field: 'drain_delay_off', value: 0 }, 
                { field: 'drain_pulse_width', value: 0 }, 
                { field: 'food_delay_on', value: 0 }, 
                { field: 'food_delay_off', value: 0 }, 
                { field: 'food_pulse_width', value: 0 }, 
                { field: 'LED_delay_on', value: 0 }, 
                { field: 'LED_delay_off', value: 0 }, 
                { field: 'LED_pulse_width', value: 0 }] 
        };
    },
    mounted() {
        // This is a alifecycle function. It is called everytime the component "mounts"
        // A mount == reload, essentially
        // Same old http request as in window.onload except...
        var req = new XMLHttpRequest();
        req.onreadystatechange = () => {
            // Pass an anonymous function here so that we don't bind 'this'. We don't want to add complexity
            if (req.readyState != 4) return; // State 4 is DONE
            const jsonResp = JSON.parse(req.responseText); //Get JSON so we can interact with the data
            this.arr.forEach(field => {
                // Really hacky way to do this based on that mock data I defined above.. It shouldn't stay like this
                // Regardless... 
                // This splits the fields into an array 'LED_delay_on' -> ['LED','delay','on']
                let splitFields = field.field.split('_')
                // This assigns variables root and subField with some syntactic sugar
                let [root,subField] = [splitFields[0], splitFields.splice(1).join('_')] //Splice removes items from an array at the given index for the given number of indices. But if not given a second argument, returns the array starting at the supplied index
                // Ternary if it has a nested field, use the root. Else just field
                field.value = root ? jsonResp[root][subField] : jsonResp[subField];
            });
        };
        req.open("GET", "http://192.168.50.36:5000/query_device/2"); // Probably want to get the device ID programmatically
        req.send();
    },
    submit() {
        //Use our saved data object and send it to the API (We don't use any DOM access here)
        console.log("HTTP REQUEST GOES HERE");
    },
    // template is the HTML part of the component. @submit.prevent="submit" does just what you think, prevents default submit. @click is a vue wrapper for onclick and submit is the name of the function being called on click
    template: `
    <div id="app">
        <form @submit.prevent="submit">

            <!-- v-for is the loop that makes multiple input groups
              -- "item in arr" -> arr is the name of the data object defined in the data() function, above
              -->
            <div v-for="item in arr" class="input-group mb-3">

                <!-- What you see here is string interpolation. e.g. {{myJSValue}}  
                  -- The item.field "type" is also defined above
                  -->
                <span class="input-group-text" id="basic-addon3">{{item.field}}: </span>

                <!-- The v-model is a directive that 2 way binds item.value to this input field. 
                  -- i.e. if this form updates, the underlying data is updated and vice-versa
                  -->
                <input type="text" class="form-control" v-model="item.value">
            </div>
            <button @click="submit" class="btn btn-primary">Submit</button>
        </form>
    </div>`
}