// This file is the entirety of the app Component
// Vue components have data and lifecycle management functions
// Interact with the components only through lifecycle functions, never through the DOM e.g. getElementById and element.addClass("myClass")

export default {
    props: {
        "system_name": String,
        "delay_on": Number,
        "delay_off": Number,
        "pulse_width": Number,

    },
    data() {
        return {
            data: {
                system_name: undefined,
                delay_off: undefined,
                delay_on: undefined,
                pulse_width: undefined
            }
        }
    },
    mounted() {
        this.data = {
            system_name: this.system_name,
            delay_off: this.delay_off,
            delay_on: this.delay_on,
            pulse_width: this.pulse_width
        }
    },
    methods: {

    },
    // template is the HTML part of the component. @submit.prevent="submit" does just what you think, prevents default submit. @click is a vue wrapper for onclick and submit is the name of the function being called on click
    template: `
    <div id="system">
        <div class="input-group mb-3">
            <span class="input-group-text" id="basic-addon3">{{ this.data.system_name }}: Delay On</span>
            <input type="number" class="form-control" 
            v-model="this.data.delay_on"
            v-on:keypress="$emit('update', this.data)"
            v-on:paste="$emit('update', this.data)"
            v-on:input="$emit('update', this.data)">
        </div>
        <div class="input-group mb-3">
            <span class="input-group-text" id="basic-addon3">{{ this.data.system_name }}: Delay Off</span>
            <input type="number" class="form-control" 
            v-model="this.data.delay_off"
            v-on:keypress="$emit('update', this.data)"
            v-on:paste="$emit('update', this.data)"
            v-on:input="$emit('update', this.data)">
        </div>
        <div class="input-group mb-3">
            <span class="input-group-text" id="basic-addon3">{{ this.data.system_name }}: Pulse Width</span>
            <input type="number" class="form-control" 
            v-model="this.data.pulse_width"
            v-on:keypress="$emit('update', this.data)"
            v-on:paste="$emit('update', this.data)"
            v-on:input="$emit('update', this.data)">
        </div>  
    </div>`
}